<h1 align="center"><a href="#">V·2</a></h1>
<p align="center">
3D game engine/framework in C.<br/>
<br/>
<img src="https://i.imgur.com/AIEIWDk.png"              /><!--cubemap--><br/>
<img src="https://i.imgur.com/ipAYky8.png" width="204px"/><!--sprite-->
<img src="https://i.imgur.com/NlSb4ip.png" width="204px"/><!--sprite3d-->
<img src="https://i.imgur.com/rmdkSiB.png" width="204px"/><!--tiled-->
<img src="https://i.imgur.com/2BfIAFD.png" width="204px"/><!--model-->
<img src="https://i.imgur.com/Sskr5pS.png" width="204px"/><!--editor-->
<img src="https://i.imgur.com/GsgOZhu.png" width="204px"/><!--fx-->
<img src="https://i.imgur.com/24oKvLj.png" width="204px"/><!--shadertoy-->
<img src="https://i.imgur.com/F9ix0Zf.png" width="204px"/><!--shadows-->
</p>

## About
- 3D game engine v2, written in C.
- v2 is a major overhaul from [previous v1 engine](https://github.com/fwk3d/v1).
- v2 is faster, smaller and stronger than v1.
- v2 is a work-in-progress, though. [Changelog](CHANGELOG.md).

## Features and wishlist
- [x] ~~Rich build system, Royaltie fee, Licensing clauses, Full featured, Fast, Modern C++~~.
- [x] Batch makefile, Free, Unlicensed, Small, Naive, Simple C.
- [ ] TBD.

## Quickstart
- Double-click `MAKE.bat`. Launch `hello.exe` when done.
- Alternatively,
  - `make [asan] [debug|devel|release|retail] [embed] file1.c file2.cc ...` for custom builds.
- Other commands,
  - `make tidy` to clean environment.
  - `make sync` to sync repo to latest.
  - `make plug` to un/install extensions.
- Tested with VS2019 and VS2022 installations.

## Extensions💙
- Currently, [all these extensions are hosted on github](https://github.com/search?q=topic%3Aext-v2&type=repositories&s=updated&o=desc).
- It is planned that the hello demo will be used to un/install extensions in the future.
- For now, you can un/install extensions via `make plug` script:
  - `make plug dir`
  - `make plug [add|del|syn] *`
  - `make plug [add|del|syn] user/repo`
  - `make plug [add|del|syn] @filelist.txt`

## Credits💜
**Artwork and demos**
[Butch](https://opengameart.org/users/buch "for golden ui (CC-BY-SA 3.0)"),
[David Lam](https://en.wikipedia.org/wiki/Tokamak_(software) "for tokamak physics engine (ZLIB)"),
[Dean Evans, Raijin](https://youtu.be/RRvYkrrpMKo?t=147 "for their Map song (c)"),
[FMS_Cat](https://gist.github.com/FMS-Cat/a1ccea3ce866c34706084e3526204f4f "for nicest VHS/VCR shader around (MIT)"),
[Goblin165cm](https://sketchfab.com/3d-models/halloween-little-witch-ccc023590bfb4789af9322864e42d1ab "for witch 3D model (CC BY 4.0)"),
[Nuulbee](https://sketchfab.com/3d-models/kgirls01-d2f946f58a8040ae993cda70c97b302c "for kgirls01 3D model (CC BY-NC-ND 4.0)"),
[Pixel Frog](https://pixelfrog-assets.itch.io/ "for their lovely asesprites (CC0)"),
[Quaternius](https://www.patreon.com/quaternius "for their lovely 3D robots (CC0)"),
[Rotting Pixels](https://opengameart.org/content/2d-castle-platformer-tileset-16x16 "for castle-tileset (CC0)"),
[Tom Lewandowski](https://QuestStudios.com "for their MIDI recordings (c)"),
[Rye Terrell](https://github.com/wwwtyro/glsl-atmosphere "for nicest rayleigh/mie scattering shader around (CC0)"),
[Rxi](https://github.com/rxi/autobatch "for their lovely sprites & cats demo (MIT)"),
**Tools**
[Aaron Barany](https://github.com/akb825/Cuttlefish "for cuttlefish (APACHE2)"),
[Andreas Mantler](https://github.com/ands/sproutline/ "for sproutline (PD)"),
[Arseny Kapoulkine](https://github.com/zeux/pugixml/ "for pugixml (MIT)"),
[Assimp authors](https://github.com/assimp/assimp "for assimp (BSD3)"),
[Bernhard Schelling](https://github.com/schellingb/TinySoundFont "for tml.h (Zlib) and tsf.h (MIT)"),
[FFMPEG authors](https://www.ffmpeg.org/ "for ffmpeg (LGPL21)"),
[Imagination](https://developer.imaginationtech.com/pvrtextool/ "for pvrtextoolcli (ITL)"),
[Krzysztof Gabis](https://github.com/kgabis/ape "for split.py/join.py (MIT)"),
[Lee Salzman](https://github.com/lsalzman/iqm/tree/5882b8c32fa622eba3861a621bb715d693573420/demo "for iqm.cpp (PD)"),
[Leon Bottou](https://github.com/facebookresearch/CParser "for lcpp (MIT)"),
[Martín Lucas Golini](https://github.com/SpartanJ/eepp/commit/8552941da19380d7a629c4da80a976aec5d39e5c "for emscripten-fs.html (CC0)"),
[Mattias Gustavsson](https://github.com/mattiasgustavsson/libs "for mid.h (PD)"),
[Morgan McGuire](https://casual-effects.com/markdeep/ "for markdeep (BSD2)"),
[Olivier Lapicque, Konstanty Bialkowski](https://github.com/Konstanty/libmodplug "for libmodplug (PD)"),
[Polyglot Team](https://docs.google.com/spreadsheets/d/17f0dQawb-s_Fd7DHgmVvJoEGDMH_yoSd8EYigrb0zmM/edit "for polyglot gamedev (CC0)"),
[Randy Gaul](https://github.com/RandyGaul/cute_headers "for cute_asesprite (PD)"),
[Rxi, Adam Harrison](https://github.com/rxi/lite "for the exquisite lite editor (MIT)"),
[Sean Barrett et al.](https://github.com/nothings/stb "for stbiresize, stbrectpack (PD)"),
[Sepehr Taghdisian](https://github.com/septag/atlasc/ "for glslcc+atlasc (BSD2)"),
[Tildearrow](https://github.com/tildearrow/furnace/ "for Furnace (GPL2)"),
[Tomas Pettersson](http://www.drpetter.se/ "for sfxr (PD)"),
[Tor Andersson](https://github.com/ccxvii/asstools "for assiqe.c (BSD)"),
[Wael El Oraiby](https://github.com/eloraiby/delaunay "for delaunay.c (AGPL3)"),
**Runtime**
[Andreas Mantler](https://github.com/ands "for lightmapper and math library (PD)"),
[Barerose](https://github.com/barerose "for swrap (CC0) and math library (CC0)"),
[Camilla Löwy](https://github.com/elmindreda "for glfw3 and gleq (Zlib)"),
[Dave Rand](https://tools.ietf.org/html/rfc1978 "for ppp (PD)"),
[David Herberth](https://github.com/dav1dde/ "for glad generated code (PD)"),
[David Reid](https://github.com/mackron "for miniaudio (PD)"),
[Dominic Szablewski](https://github.com/phoboslab/pl_mpeg "for pl_mpeg (MIT)"),
[Dominik Madarász](https://github.com/zaklaus "for json5 parser (PD)"),
[Eduard Suica](https://github.com/eduardsui/tlse "for tlse (PD)"),
[Evan Wallace](https://github.com/evanw "for their math library (CC0)"), 
[Gargaj+cce/Peisik](https://github.com/gargaj/foxotron "for Foxotron/PBR shaders (UNLICENSE)"),
[Guilherme Lampert](https://github.com/glampert "for their math library (PD)"), 
[Guillaume Vareille](http://tinyfiledialogs.sourceforge.net "for tinyfiledialogs (ZLIB)"),
[Haruhiko Okumura](https://oku.edu.mie-u.ac.jp/~okumura/compression/ "for lzss (PD)"),
[Igor Pavlov](https://www.7-zip.org/ "for LZMA (PD)"),
[Ilya Muravyov](https://github.com/encode84 "for bcm, balz, crush, ulz, lz4x (PD)"),
[James R. McKaskill](https://github.com/jmckaskill/luaffi "for luaffi (MIT)"),
[Jon Olick](https://www.jonolick.com/ "for jo_mp1 and jo_mpeg (PD)"),
[Joonas Pihlajamaa](https://github.com/jokkebk/JUnzip "for JUnzip library (PD)"),
[Juliette Focault](https://github.com/juliettef/IconFontCppHeaders/blob/main/IconsMaterialDesign.h "for the generated MD header (ZLIB)"),
[Kristoffer Grönlund](https://github.com/krig "for their math library (CC0)"), 
[Lee Salzman](https://github.com/lsalzman/iqm/tree/5882b8c32fa622eba3861a621bb715d693573420/demo "for IQM spec & player (PD)"),
[Lee Salzman, V.Hrytsenko, D.Madarász](https://github.com/zpl-c/enet/ "for enet (MIT)"),
[Libtomcrypt](https://github.com/libtom/libtomcrypt "for libtomcrypt (Unlicense)"),
[Lua authors](https://www.lua.org/ "for Lua language (MIT)"),
[Mattias Gustavsson](https://github.com/mattiasgustavsson/libs "for thread.h and https.h (PD)"),
[Mattias Jansson](https://github.com/mjansson/rpmalloc "for rpmalloc (PD)"),
[Micha Mettke](https://github.com/vurtun "for nuklear and their math library (PD)"),
[Michael Galetzka](https://github.com/Cultrarius/Swarmz "for swarmz (UNLICENSE)"),
[Morten Vassvik](https://github.com/vassvik/mv_easy_font "for mv_easy_font (Unlicense)"),
[Mārtiņš Možeiko](https://gist.github.com/mmozeiko/68f0a8459ef2f98bcd879158011cc275 "for A* pathfinding (PD)"),
[Omar Cornut, vaiorabbit](https://github.com/ocornut/imgui/pull/3627 "for tables of unicode ranges (MIT-0)"),
[Peter Schulman, Chris Willcocks, Dmitry Hrabrov](https://github.com/vurtun/nuklear "for nuklear patches (PD)"),
[Rabia Alhaffar](https://github.com/Rabios/ice_libs "for ice_batt.h (PD)"),
[Randy Gaul](https://web.archive.org/*/http://www.randygaul.net/wp-content/uploads/2021/04/handle_table.cpp "for HandleTable (PD)"),
[Rich Geldreich](https://github.com/richgel999/miniz "for miniz (PD)"),
[Ross Williams](http://ross.net/compression/lzrw3a.html "for lzrw3a (PD)"),
[Samuli Raivio](https://github.com/bqqbarbhg/bq_websocket "for bq_websocket (PD)"),
[Scott Lembcke](https://github.com/slembcke/debugger.lua "for lua debugger (MIT)"),
[Sean Barrett](https://github.com/nothings "for stb_image, stb_image_write, stb_sprintf, stb_truetype and stb_vorbis (PD)"),
[Sebastian Steinhauer](https://github.com/kieselsteini "for sts_mixer (PD)"),
[Stan Melax, Cloud Wu](https://web.archive.org/web/20031204035320/http://www.melax.com/polychop/gdmag.pdf "for polychop C algorithm (PD)"),
[Stefan Gustavson](https://github.com/stegu/perlin-noise "for simplex noise (PD)"),
[Sterling Orsten](https://github.com/sgorsten "for their math library (UNLICENSE)"),
[Tor Andersson](https://github.com/ccxvii/minilibs "for xml.c (PD)"),
[Werner Stoop](engine/split/3rd_eval.h "for their expression evaluator (PD)"),
[Wolfgang Draxinger](https://github.com/datenwolf "for their math library (WTFPL2)"), 

## Unlicense
This software is released into the [public domain](https://unlicense.org/). Also dual-licensed as [0-BSD](https://opensource.org/licenses/0BSD) or [MIT (No Attribution)](https://github.com/aws/mit-0) for those countries where public domain is a concern (sigh). Any contribution to this repository is implicitly subjected to the same release conditions aforementioned.

## Links
Still looking for alternatives? Get some: [amulet](https://github.com/ianmaclarty/amulet), [aroma](https://github.com/leafo/aroma/), [astera](https://github.com/tek256/astera), [blendelf](https://github.com/jesterKing/BlendELF), [bullordengine](https://github.com/MarilynDafa/Bulllord-Engine), [candle](https://github.com/EvilPudding/candle), [cave](https://github.com/kieselsteini/cave), [chickpea](https://github.com/ivansafrin/chickpea), [corange](https://github.com/orangeduck/Corange), [cute](https://github.com/RandyGaul/cute_framework), [dos-like](https://github.com/mattiasgustavsson/dos-like), [ejoy2d](https://github.com/ejoy/ejoy2d), [exengine](https://github.com/exezin/exengine), [game-framework](https://github.com/Planimeter/game-framework), [gunslinger](https://github.com/MrFrenik/gunslinger), [hate](https://github.com/excessive/hate), [island](https://github.com/island-org/island), [juno](https://github.com/rxi/juno), [l](https://github.com/Lyatus/L), [limbus](https://github.com/redien/limbus), [love](https://github.com/love2d/love/), [lovr](https://github.com/bjornbytes/lovr), [mini3d](https://github.com/mini3d/mini3d), [mintaro](https://github.com/mackron/mintaro), [mio](https://github.com/ccxvii/mio), [olive.c](https://github.com/tsoding/olive.c), [opensource](https://github.com/w23/OpenSource), [ouzel](https://github.com/elnormous/ouzel/), [pez](https://github.com/prideout/pez), [pixie](https://github.com/mattiasgustavsson/pixie), [punity](https://github.com/martincohen/Punity), [r96](https://github.com/badlogic/r96), [ricotech](https://github.com/dbechrd/RicoTech), [rizz](https://github.com/septag/rizz), [tigr](https://github.com/erkkah/tigr), [yourgamelib](https://github.com/duddel/yourgamelib)

<a href="https://github.com/fwk3d/v2/issues"><img alt="Issues" src="https://img.shields.io/github/issues-raw/fwk3d/v2.svg?label=Issues&logo=github&logoColor=white"/></a> <a href="https://github.com/fwk3d/v2/actions/workflows/build.yml"><img alt="Build status" src="https://github.com/fwk3d/v2/actions/workflows/build.yml/badge.svg"/></a> <a href="https://discord.gg/UpB7nahEFU"><img alt="Discord" src="https://img.shields.io/discord/270565488365535232?color=5865F2&label=Chat&logo=discord&logoColor=white"/></a> 

<!-- 
🟥🟧🟨🟩🟦🟪❤️🧡💛💚💙💜

What, When, Where, Why, How, Which, Who, and Whom
-->
