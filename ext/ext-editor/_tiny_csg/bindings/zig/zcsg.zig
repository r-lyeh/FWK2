
const std = @import("std");
const options = @import("zcsg_options");
const c = @cImport({
    @cInclude("ccsg.h");
});

//--------------------------------------------------------------------------------------------------
// Init/deinit and global state
//--------------------------------------------------------------------------------------------------
const SizeAndAlignment = packed struct(u64) {
    size: u48,
    alignment: u16,
};
var mem_allocator: ?std.mem.Allocator = null;
var mem_allocations: ?std.AutoHashMap(usize, SizeAndAlignment) = null;
var mem_mutex: std.Thread.Mutex = .{};
const mem_alignment = 16;

pub fn init_allocator(allocator: std.mem.Allocator) !void {
    std.debug.assert(options.use_custom_alloc);
    if (!options.use_custom_alloc) return;

    std.debug.assert(mem_allocator == null and mem_allocations == null);

    mem_allocator = allocator;
    mem_allocations = std.AutoHashMap(usize, SizeAndAlignment).init(allocator);
    try mem_allocations.?.ensureTotalCapacity(32);

    c.CCSG_RegisterCustomAllocator(zcsgAlloc, zcsgFree, zcsgAlignedAlloc, zcsgFree);
}

pub fn deinit_allocator() void {
    std.debug.assert(options.use_custom_alloc);
    if (!options.use_custom_alloc) return;

    mem_allocations.?.deinit();
    mem_allocations = null;
    mem_allocator = null;
}

//--------------------------------------------------------------------------------------------------
// Memory allocation
//--------------------------------------------------------------------------------------------------
fn zcsgAlloc(size: usize) callconv(.C) ?*anyopaque {
    mem_mutex.lock();
    defer mem_mutex.unlock();

    const ptr = mem_allocator.?.rawAlloc(
        size,
        std.math.log2_int(u29, @as(u29, @intCast(mem_alignment))),
        @returnAddress(),
    );
    if (ptr == null) @panic("zcsg: out of memory");

    mem_allocations.?.put(
        @intFromPtr(ptr),
        .{ .size = @as(u48, @intCast(size)), .alignment = mem_alignment },
    ) catch @panic("zcsg: out of memory");

    return ptr;
}

fn zcsgAlignedAlloc(size: usize, alignment: usize) callconv(.C) ?*anyopaque {
    mem_mutex.lock();
    defer mem_mutex.unlock();

    const ptr = mem_allocator.?.rawAlloc(
        size,
        std.math.log2_int(u29, @as(u29, @intCast(alignment))),
        @returnAddress(),
    );
    if (ptr == null) @panic("zcsg: out of memory");

    mem_allocations.?.put(
        @intFromPtr(ptr),
        .{ .size = @as(u32, @intCast(size)), .alignment = @as(u16, @intCast(alignment)) },
    ) catch @panic("zcsg: out of memory");

    return ptr;
}

fn zcsgFree(maybe_ptr: ?*anyopaque) callconv(.C) void {
    if (maybe_ptr) |ptr| {
        mem_mutex.lock();
        defer mem_mutex.unlock();

        const info = mem_allocations.?.fetchRemove(@intFromPtr(ptr)).?.value;

        const mem = @as([*]u8, @ptrCast(ptr))[0..info.size];

        mem_allocator.?.rawFree(
            mem,
            std.math.log2_int(u29, @as(u29, @intCast(info.alignment))),
            @returnAddress(),
        );
    }
}

//--------------------------------------------------------------------------------------------------
// Reinterpreted Types - Must maintain these in sync with csg types
//--------------------------------------------------------------------------------------------------
pub const Volume = i32;
pub const Vec3 = [3]f32;
pub const Mat4 = [16]f32;

pub const Plane = extern struct {
    normal: Vec3,
    offset: f32,

    comptime { std.debug.assert(@sizeOf(@This()) == @sizeOf(c.CCSG_Plane)); }
};

pub const Face = extern struct {
    plane: *const Plane,
    _pad0: [3]*const anyopaque,
    _pad1: [3]*const anyopaque,

    comptime { std.debug.assert(@sizeOf(@This()) == @sizeOf(c.CCSG_Face)); }

    pub fn getVertices(face: *const Face) ?[]const Vertex {
        var ptr: [*c]Vertex = null;
        const len = c.CCSG_Face_GetVerticesPtr(
            @as(*const c.CCSG_Face, @ptrCast(face)),
            @as([*c][*c] c.CCSG_Vertex, @ptrCast(&ptr)),
        );
        if (ptr) |array| {
            return array[0..len];
        }
        return null;
    }
    pub fn getFragments(face: *const Face) ?[]const Fragment {
        var ptr: [*c]Fragment = null;
        const len = c.CCSG_Face_GetFragmentsPtr(
            @as(*const c.CCSG_Face, @ptrCast(face)),
            @as([*c][*c] c.CCSG_Fragment, @ptrCast(&ptr)),
        );
        if (ptr) |array| {
            return array[0..len];
        }
        return null;
    }
};

pub const Fragment = extern struct {
    face: *Face,
    _pad0: [3]*const anyopaque,
    front_volume: Volume,
    back_volume: Volume,
    front_brush: *Brush,
    back_brush: *Brush,
    _pad1: i32,

    comptime { std.debug.assert(@sizeOf(@This()) == @sizeOf(c.CCSG_Fragment)); }

    pub fn getVertices(fragment: *const Fragment) ?[]const Vertex {
        var ptr: [*c]Vertex = null;
        const len = c.CCSG_Fragment_GetVerticesPtr(
            @as(*const c.CCSG_Fragment, @ptrCast(fragment)),
            @as([*c][*c] c.CCSG_Vertex, @ptrCast(&ptr)),
        );
        if (ptr) |array| {
            return array[0..len];
        }
        return null;
    }
};

pub const Ray = extern struct {
    origin: Vec3 = undefined,
    direction: Vec3 = undefined,

    comptime { std.debug.assert(@sizeOf(@This()) == @sizeOf(c.CCSG_Ray)); }
};

pub const RayHit = extern struct {
    brush: *Brush,
    face: *Face,
    fragment: *Fragment,
    parameter: f32,
    position: Vec3,

    comptime { std.debug.assert(@sizeOf(@This()) == @sizeOf(c.CCSG_RayHit)); }
};

pub const Box = extern struct {
    min: Vec3 = .{ 0, 0, 0 },
    max: Vec3 = .{ 0, 0, 0 },

    pub fn encompass(box: *Box, other: Box) void {
        const box_min: @Vector(3, f32) = box.min;
        const box_max: @Vector(3, f32) = box.max;
        const other_min: @Vector(3, f32) = other.min;
        const other_max: @Vector(3, f32) = other.max;
        box.min = @min(box_min, other_min);
        box.max = @max(box_max, other_max);
    }

    comptime { std.debug.assert(@sizeOf(@This()) == @sizeOf(c.CCSG_Box)); }
};

pub const Vertex = extern struct {
    position: Vec3,
    _pad0: [3]*const anyopaque,

    comptime { std.debug.assert(@sizeOf(@This()) == @sizeOf(c.CCSG_Vertex)); }
};

pub const Triangle = extern struct {
    i: i32,
    j: i32,
    k: i32,

    comptime { std.debug.assert(@sizeOf(@This()) == @sizeOf(c.CCSG_Triangle)); }
};

//--------------------------------------------------------------------------------------------------
// VolumeOperation
//--------------------------------------------------------------------------------------------------
pub const VolumeOperation = opaque {
    pub fn initFill(volume: Volume) *VolumeOperation {
        return @as(*VolumeOperation, @ptrCast(c.CCSG_MakeFillOperation(volume)));
    }
    pub fn initConvert(from: Volume, to: Volume) *VolumeOperation {
        return @as(*VolumeOperation, @ptrCast(c.CCSG_MakeConvertOperation(from, to)));
    }
    pub fn deinit(operation: *VolumeOperation) void {
        c.CCSG_VolumeOperation_Destroy(@as(*c.CCSG_VolumeOperation, @ptrCast(operation)));
    }
};

//--------------------------------------------------------------------------------------------------
// World
//--------------------------------------------------------------------------------------------------
pub const World = opaque {
    pub fn init() *World {
        return @as(*World, @ptrCast(c.CCSG_World_Create()));
    }
    pub fn deinit(world: *World) void {
        c.CCSG_World_Destroy(@as(*c.CCSG_World, @ptrCast(world)));
    }

    pub fn first(world: *World) ?*Brush {
        const result = c.CCSG_World_First(@as(*c.CCSG_World, @ptrCast(world)));
        return if (result == null) null else @as(*Brush, @ptrCast(result));
    }
    pub fn next(world: *World, brush: *Brush) ?*Brush {
        const result = c.CCSG_World_Next(@as(*c.CCSG_World, @ptrCast(world)), @as(*c.CCSG_Brush, @ptrCast(brush)));
        return if (result == null) null else @as(*Brush, @ptrCast(result));
    }
    pub fn remove(world: *World, brush: *Brush) void {
        c.CCSG_World_Remove(@as(*c.CCSG_World, @ptrCast(world)), @as(*c.CCSG_Brush, @ptrCast(brush)));
    }
    pub fn add(world: *World) *Brush {
        return @as(*Brush, @ptrCast(c.CCSG_World_Add(@as(*c.CCSG_World, @ptrCast(world)))));
    }

    pub fn rebuild(world: *World) *BrushSet {
        return @as(*BrushSet, @ptrCast(c.CCSG_World_Rebuild(@as(*c.CCSG_World, @ptrCast(world)))));
    }

    pub fn queryPoint(world: *World, point: Vec3) *BrushList {
        return @as(*BrushList, @ptrCast(c.CCSG_World_QueryPoint(
            @as(*c.CCSG_World, @ptrCast(world)),
            @as(*const c.CCSG_Vec3, @ptrCast(&point)),
        )));
    }
    pub fn queryBox(world: *World, box: Box) *BrushList {
        return @as(*BrushList, @ptrCast(c.CCSG_World_QueryBox(
            @as(*c.CCSG_World, @ptrCast(world)),
            @as(*const c.CCSG_Box, @ptrCast(&box)),
        )));
    }
    pub fn queryRay(world: *World, ray: Ray) *RayHitList {
        return @as(*RayHitList, @ptrCast(c.CCSG_World_QueryRay(
            @as(*c.CCSG_World, @ptrCast(world)),
            @as(*const c.CCSG_Ray, @ptrCast(&ray)),
        )));
    }
    pub fn qeryFrustum(world: *World, view_projection: Mat4) *BrushList {
        return @as(*BrushList, @ptrCast(c.CCSG_World_QueryFrustum(
            @as(*c.CCSG_World, @ptrCast(world)),
            @as(*const c.CCSG_Mat4, @ptrCast(&view_projection)),
        )));
    }
};

//--------------------------------------------------------------------------------------------------
// Brush
//--------------------------------------------------------------------------------------------------
pub const Brush = opaque {
    pub fn setPlanes(brush: *Brush, planes: []const Plane) void {
        c.CCSG_Brush_SetPlanes(
            @as(*c.CCSG_Brush, @ptrCast(brush)),
            @as(*const c.CCSG_Plane, @ptrCast(planes.ptr)),
            planes.len,
        );
    }

    pub fn getPlanes(brush: *const Brush) ?[]const Plane {
        const vec = c.CCSG_Brush_GetPlanes(@as(*const c.CCSG_Brush, @ptrCast(brush))) orelse return null;
        var ptr: [*c]Plane = null;
        const len = c.CCSG_PlaneVec_GetPtr(vec, @as([*c][*c] c.CCSG_Plane, @ptrCast(&ptr)));
        if (ptr) |array| {
            return array[0..len];
        }
        return null;
    }

    pub fn setVolumeOperation(brush: *Brush, op: *const VolumeOperation) void {
        c.CCSG_Brush_SetVolumeOperation(
            @as(*c.CCSG_Brush, @ptrCast(brush)),
            @as(*const c.CCSG_VolumeOperation, @ptrCast(op)),
        );
    }

    pub fn getFaces(brush: *const Brush) ?[]const Face {
        const vec = c.CCSG_Brush_GetFaces(@as(*const c.CCSG_Brush, @ptrCast(brush))) orelse return null;
        var ptr: [*c]Face = null;
        const len = c.CCSG_FaceVec_GetPtr(vec, @as([*c][*c] c.CCSG_Face, @ptrCast(&ptr)));
        if (ptr) |array| {
            return array[0..len];
        }
        return null;
    }

    pub fn getBox(brush: *const Brush) Box {
        const c_box = c.CCSG_Brush_GetBox(@as(*const c.CCSG_Brush, @ptrCast(brush)));
        return Box {
            .min = c_box.min,
            .max = c_box.max,
        };
    }

    pub fn getUserData(brush: *const Brush) ?*anyopaque {
        return c.CCSG_Brush_GetUserData(@as(*const c.CCSG_Brush, @ptrCast(brush)));
    }

    pub fn setUserData(brush: *Brush, data: ?*anyopaque) void {
        c.CCSG_Brush_SetUserData(@as(*c.CCSG_Brush, @ptrCast(brush)), data);
    }
};

//--------------------------------------------------------------------------------------------------
// STL Container Wrappers
//--------------------------------------------------------------------------------------------------
pub const BrushSet = opaque {
    pub fn deinit(set: *BrushSet) void {
        c.CCSG_BrushSet_Destroy(@as(*c.CCSG_BrushSet, @ptrCast(set)));
    }
    pub fn iterator(set: *BrushSet) *Iterator {
        return @as(*Iterator, @ptrCast(c.CCSG_BrushSet_Iterator_Begin(@as(*c.CCSG_BrushSet, @ptrCast(set)))));
    }

    pub const Iterator = opaque {
        pub fn deinit(self: *Iterator) void {
            c.CCSG_BrushSet_Iterator_Destroy(@as(*c.CCSG_BrushSet_Iterator, @ptrCast(self)));
        }
        pub fn next(self: *Iterator, set: *BrushSet) ?*const Brush {
            const current = c.CCSG_BrushSet_Iterator_Read(@as(*const c.CCSG_BrushSet_Iterator, @ptrCast(self)));
            const exists = c.CCSG_BrushSet_Iterator_Next(
                @as(*c.CCSG_BrushSet, @ptrCast(set)),
                @as(*c.CCSG_BrushSet_Iterator, @ptrCast(self)),
            );
            if (exists == 0) return null;
            return @as(*const Brush, @ptrCast(current));
        }
    };
};

pub const BrushList = opaque {
    pub fn deinit(list: *BrushList) void {
        c.CCSG_BrushVec_Destroy(@as(*c.CCSG_BrushVec, @ptrCast(list)));
    }
    pub fn getSlice(list: *BrushList) ?[]const *Brush {
        var ptr: [*c]*Brush = null;
        const len = c.CCSG_BrushVec_GetPtr(
            @as(*const c.CCSG_BrushVec, @ptrCast(list)),
            @as([*c][*c] *c.CCSG_Brush, @ptrCast(&ptr)),
        );
        if (ptr) |array| {
            return array[0..len];
        }
        return null;
    }
};

pub const RayHitList = opaque {
    pub fn deinit(list: *RayHitList) void {
        c.CCSG_RayHitVec_Destroy(@as(*c.CCSG_RayHitVec, @ptrCast(list)));
    }
    pub fn getSlice(list: *RayHitList) ?[]const RayHit {
        var ptr: [*c]RayHit = null;
        const len = c.CCSG_RayHitVec_GetPtr(
            @as(*const c.CCSG_RayHitVec, @ptrCast(list)),
            @as([*c][*c] c.CCSG_RayHit, @ptrCast(&ptr)),
        );
        if (ptr) |array| {
            return array[0..len];
        }
        return null;
    }
};

pub const TriangleList = opaque {
    pub fn deinit(list: *TriangleList) void {
        c.CCSG_TriangleVec_Destroy(@as(*c.CCSG_TriangleVec, @ptrCast(list)));
    }
    pub fn getSlice(list: *TriangleList) ?[]const Triangle {
        var ptr: [*c]Triangle = null;
        const len = c.CCSG_TriangleVec_GetPtr(
            @as(*const c.CCSG_TriangleVec, @ptrCast(list)),
            @as([*c][*c] c.CCSG_Triangle, @ptrCast(&ptr)),
        );
        if (ptr) |array| {
            return array[0..len];
        }
        return null;
    }
};

//--------------------------------------------------------------------------------------------------
// Misc.
//--------------------------------------------------------------------------------------------------
pub fn triangulate(fragment: *const Fragment) *TriangleList {
    return @as(*TriangleList, @ptrCast(c.CCSG_Triangulate(@as(*const c.CCSG_Fragment, @ptrCast(fragment)))));
}

//--------------------------------------------------------------------------------------------------
// Tests
//--------------------------------------------------------------------------------------------------
const expect = std.testing.expect;

test "decls" {
    std.testing.refAllDeclsRecursive(@This());
}

extern fn CCSG_Test_HelloWorld() u32;
test "ccsg.helloworld" {
    if (options.use_custom_alloc) try init_allocator(std.testing.allocator);
    const ret = CCSG_Test_HelloWorld();
    try expect(ret == 0);
    if (options.use_custom_alloc) deinit_allocator();
}

test "helloworld" {
    if (options.use_custom_alloc) try init_allocator(std.testing.allocator);
    const world = World.init();
    world.deinit();
    if (options.use_custom_alloc) deinit_allocator();
}

test "square_torus" {
    if (options.use_custom_alloc) try init_allocator(std.testing.allocator);
    defer { if (options.use_custom_alloc) deinit_allocator(); }

    const space_brush_op: Volume = 0;
    const solid_brush_op: Volume = 1;

    const space_volume_op = VolumeOperation.initFill(space_brush_op);
    defer space_volume_op.deinit();

    const solid_volume_op = VolumeOperation.initFill(solid_brush_op);
    defer solid_volume_op.deinit();

    const csg_world = World.init();
    defer csg_world.deinit();

    const brush_0 = csg_world.add();
    defer csg_world.remove(brush_0);
    brush_0.setVolumeOperation(solid_volume_op);

    const planes_0: [6]Plane = .{
        .{ .normal = .{ 0, 0, 1 }, .offset = -30 },
        .{ .normal = .{ 0, 0, -1 }, .offset = 20 },
        .{ .normal = .{ 1, 0, 0 }, .offset = -10 },
        .{ .normal = .{ -1, 0, 0 }, .offset = -10 },
        .{ .normal = .{ 0, 1, 0 }, .offset = -10 },
        .{ .normal = .{ 0, -1, 0 }, .offset = -10 },
    };
    brush_0.setPlanes(&planes_0);

    const brush_1 = csg_world.add();
    defer csg_world.remove(brush_1);
    brush_1.setVolumeOperation(space_volume_op);

    const planes_1: [6]Plane = .{
        .{ .normal = .{ 0, 0, 1 }, .offset = -30 },
        .{ .normal = .{ 0, 0, -1 }, .offset = 20 },
        .{ .normal = .{ 1, 0, 0 }, .offset = -5 },
        .{ .normal = .{ -1, 0, 0 }, .offset = -5 },
        .{ .normal = .{ 0, 1, 0 }, .offset = -5 },
        .{ .normal = .{ 0, -1, 0 }, .offset = -5 },
    };
    brush_1.setPlanes(&planes_1);

    const changed_brushes = csg_world.rebuild();
    defer changed_brushes.deinit();

    var points = std.ArrayList([3]f32).init(std.testing.allocator);
    defer points.deinit();

    var indices = std.ArrayList(u32).init(std.testing.allocator);
    defer indices.deinit();

    const brush_iterator = changed_brushes.iterator();
    defer brush_iterator.deinit();
    while (brush_iterator.next(changed_brushes)) |brush| {
        const faces = brush.getFaces() orelse continue;
        for (faces) |face| {
            const fragments = face.getFragments() orelse continue;
            for (fragments) |fragment| {
                if (fragment.back_volume == fragment.front_volume) continue;
                const vertices = fragment.getVertices() orelse continue;

                const index_head = @as(u32, @intCast(points.items.len));
                for (vertices) |vertex| { try points.append(vertex.position); }

                const triangle_list = triangulate(&fragment);
                defer triangle_list.deinit();

                const triangles = triangle_list.getSlice() orelse continue;
                for (triangles) |triangle| {
                    if (fragment.back_volume == space_brush_op) {
                        try indices.append(@as(u32, @intCast(triangle.i)) + index_head);
                        try indices.append(@as(u32, @intCast(triangle.k)) + index_head);
                        try indices.append(@as(u32, @intCast(triangle.j)) + index_head);
                    } else {
                        try indices.append(@as(u32, @intCast(triangle.i)) + index_head);
                        try indices.append(@as(u32, @intCast(triangle.j)) + index_head);
                        try indices.append(@as(u32, @intCast(triangle.k)) + index_head);
                    }
                }
            }
        }
    }

    try expect(points.items.len == 64);
    try expect(indices.items.len == 96);
}