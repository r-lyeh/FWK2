-- this will run on vanilla luajit.exe, provided that C.dll and this file are all present in same folder
local ffi = require("ffi")

-- create 75% sized + MSAAx2 anti-aliased window
C.app_create(75.0, C.APP_MSAA8)

-- set window title
--C.app_title("hello luajit")

-- config girl
local girl = C.model('kgirl/kgirls01.iqm', 0)
local girl_frame = 0
local girl_pivot = ffi.new("mat44") -- ffi.new("mat44") C.mat44()
--C.rotationq44(girl_pivot, C.eulerq(C.vec3(0,0,0)))
--C.scale44(girl_pivot, 2,2,2)
girl_pivot[ 1] = 2; girl_pivot[ 5]= 0; girl_pivot[ 9]= 0; girl_pivot[13]=0
girl_pivot[ 2] = 0; girl_pivot[ 6]=-2; girl_pivot[10]= 0; girl_pivot[14]=0
girl_pivot[ 3] = 0; girl_pivot[ 7]= 0; girl_pivot[11]=-2; girl_pivot[15]=0
girl_pivot[ 4] = 0; girl_pivot[ 8]= 0; girl_pivot[12]= 0; girl_pivot[16]=1

girl_pivot[ 1] = 2; girl_pivot[ 2]= 0; girl_pivot[ 3]= 0; girl_pivot[ 4]=0
girl_pivot[ 5] = 0; girl_pivot[ 6]=-2; girl_pivot[ 7]= 0; girl_pivot[ 8]=0
girl_pivot[ 9] = 0; girl_pivot[10]= 0; girl_pivot[11]=-2; girl_pivot[12]=0
girl_pivot[13] = 0; girl_pivot[14]= 0; girl_pivot[15]= 0; girl_pivot[16]=1

local pivots = ffi.new("mat44[?]", 1)
pivots[1] = girl_pivot

-- config & play music
local music = C.audio('the_entertainer.ogg',0,0) -- audio_stream('larry.mid') -- 'wrath_of_the_djinn.xm'
--C.audio_play(music, 0);
local spk = C.speaker();
C.speaker_play( spk, music, 0 );

-- config camera
local cam = C.camera()

-- main loop
while C.app_swap() == 1 do
   -- fps camera
   C.camera_freefly(cam, false)

   -- draw grid/axis
   C.ddraw_grid(0)
   C.ddraw_flush()

   -- animate girl
   local delta = C.app_delta() * 30 -- 30fps anim
   girl_frame = C.model_animate(girl, girl_frame + delta)

   -- draw girl
   C.model_render(girl, cam.proj, cam.view, pivots, 1, 0)

   -- showcase ui
   if C.ui_panel("Luajit", 0) == 1 then
      if C.ui_button("Push me") == 1 then
         C.ui_notify("Hello from Lua!","You received a notification.")
      end
      C.ui_panel_end()
   end
end
