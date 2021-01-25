@echo on

rem copy inlib library

set configure=%1
set platform=%2
set install_dir=%3
set solutionName=%4

cd ..\
set rebuild_path=%CD%
cd ..\
set src_headerPath=%CD%

set platform_short_name=x64
if %platform%==Win32 set platform_short_name=Win32

echo configure=%configure% platform=%platform% install_dir=%install_dir% pwd=%rebuild_path% solutionName=%solutionName%

rem 创建原始目录
mkdir %install_dir%

echo ====================Project:[%solutionName%]========================

set project=%solutionName%
set project_install_dir=%install_dir%\%project%

rem 创建项目目录
mkdir %project_install_dir%

set project_install_dir_header=%project_install_dir%\include
set project_install_dir_lib=%project_install_dir%\lib\%configure%
set project_install_dir_bin=%project_install_dir%\bin\%configure%
set project_install_dir_plugin=%project_install_dir%\Plugin

rem 安装头文件
mkdir %project_install_dir_header%

xcopy /a /y  %src_headerPath%\%project%\include\*.h %project_install_dir_header%

rem 安装库文件
mkdir %project_install_dir_lib%
mkdir %project_install_dir_bin%

xcopy /a /y  %rebuild_path%\%project%\%configure%\*.lib %project_install_dir_lib%
xcopy /a /y  %rebuild_path%\%project%\%configure%\*.dll %project_install_dir_bin%
xcopy /a /y  %rebuild_path%\%project%\%configure%\*.exe %project_install_dir_bin%
xcopy /a /y  %rebuild_path%\%project%\%configure%\*.pdb %project_install_dir_bin%

echo ------------------------------------------------------------------------------

cd %pwd%

pause
@echo on