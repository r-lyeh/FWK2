git clone https://github.com/kcat/openal-soft/
pushd openal-soft
git checkout 90191ed
cmake -DFORCE_STATIC_VCRT=ON -DCMAKE_BUILD_TYPE=Release ^
-DCMAKE_C_FLAGS="/MT" -DCMAKE_CXX_FLAGS="/MT" ^
-A x64 ^
-DALSOFT_TESTS=ON ^
-DALSOFT_BUILD_ROUTER=ON ^
-DALSOFT_REQUIRE_WINMM=ON ^
-DALSOFT_REQUIRE_DSOUND=ON ^
-DALSOFT_REQUIRE_WASAPI=ON

cmake --build . --config Release --verbose
popd

copy /y openal-soft\Release\*.dll x64\

for %%i in (x64\*.exe;x64\*.dll) do dumpbin /dependents %%i | find /i "CRT" 
