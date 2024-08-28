from os import environ
import os.path
import shutil
import glob
import subprocess
import sys
import warnings

if not environ.get('OUTPUT_DIR'):
    environ['OUTPUT_DIR'] = fr"{os.getcwd()}\out"
if not environ.get('KRITA_DIR'):
    if not environ.get('KRITA_INSTALLER'):
        print("ERROR: KRITA_DIR and KRITA_INSTALLER not specified, one of them must be set.")
        exit(1)

# For CI, define both KRITA_DIR and KRITA_SHELLEX.
# Do not use KRITA_SHELLEX if building outside of CI, unless you
# don't mind this script modifying the contents of KRITA_DIR.
if environ.get('KRITA_INSTALLER'):
    if environ.get('KRITA_SHELLEX'):
        print("ERROR: KRITA_SHELLEX must not be set if using KRITA_INSTALLER.")
        exit(1)

# Subroutines

def get_temp_file():
    uniqueFileName = fr"{environ.get('tmp')}\bat~{environ.get('RANDOM')}.tmp"
    if os.path.isFile(uniqueFileName):
        uniqueFileName = get_temp_file()
    return uniqueFileName

def find_on_path(variable, executable):
    os.environ[variable] = shutil.which(executable)

# Begin

print("*** Krita MSIX build script ***")

if (not environ.get('WindowsSdkDir')) and environ.get('ProgramFiles(x86)'):
    environ['WindowsSdkDir'] = fr"{environ['ProgramFiles(x86)']}\Windows Kits\10"
if os.path.isfile(environ.get('WindowsSdkDir')):
    delims = glob.glob(fr"{environ['WindowsSdkDir']}\bin\10.*")
    for dir in delims:
        if os.path.isfile(fr"{environ['WindowsSdkDir']}\bin\{dir}\x64\makepri.exe"):
            environ['MAKEPRI'] = fr"{environ['WindowsSdkDir']}\bin\{dir}\x64\makepri.exe"
        if os.path.isfile(fr"{environ['WindowsSdkDir']}\bin\{dir}\x64\makeappx.exe"):
            environ['MAKEAPPX'] = fr"{environ['WindowsSdkDir']}\bin\{dir}\x64\makeappx.exe"
        if os.path.isfile(fr"{environ['WindowsSdkDir']}\bin\{dir}\x64\signtool.exe"):
            environ['SIGNTOOL'] = fr"{environ['WindowsSdkDir']}\bin\{dir}\x64\signtool.exe"
    if (not environ['MAKEPRI']) and os.path.isfile(fr"{environ['WindowsSdkDir']}\bin\x64\makepri.exe"):
        environ['MAKEPRI'] = fr"{environ['WindowsSdkDir']}\bin\x64\makepri.exe"
    if (not environ['MAKEAPPX']) and os.path.isfile(fr"{environ['WindowsSdkDir']}\bin\x64\makeappx.exe"):
        environ['MAKEAPPX'] = fr"{environ['WindowsSdkDir']}\bin\x64\makeappx.exe"
    if (not environ['SIGNTOOL']) and os.path.isfile(fr"{environ['WindowsSdkDir']}\bin\x64\signtool.exe"):
        environ['SIGNTOOL'] = fr"{environ['WindowsSdkDir']}\bin\x64\signtool.exe"

if not environ.get('MAKEPRI'):
    print("ERROR: makepri not found")
    exit(1)

if not environ.get('MAKEAPPX'):
    print("ERROR: makeappx not found")
    exit(1)

if not environ.get('SIGNTOOL'):
    print("ERROR: signtool not found")
    exit(1)

environ['SCRIPT_DIR'] = sys.argv[0]
os.mkdir(environ['OUTPUT_DIR'])


if not environ['KRITA_DIR']:

    print("\n=== Step 0: Extract files from installer")

    if not environ.get("SEVENZIP_EXE"):
        find_on_path("SEVENZIP_EXE", "7z.exe")
    if not environ.get("SEVENZIP_EXE"):
        find_on_path("SEVENZIP_EXE", "7za.exe")
    if not environ.get("SEVENZIP_EXE"):
        os.environ["SEVENZIP_EXE"] = fr"{environ.get('ProgramFiles')}\7-Zip\7z.exe"
        if not os.path.isfile(os.environ["SEVENZIP_EXE"]):
            os.environ["SEVENZIP_EXE"] = fr"{environ.get('ProgramFiles(x86)')}\\7-Zip\\7z.exe"
        if not os.path.isfile(os.environ["SEVENZIP_EXE"]):
            warnings.warn("7-Zip not found!")
            exit(102)

    os.mkdir(fr"{environ['OUTPUT_DIR']}\installer_content")
    commandToRun = f"{environ['SEVENZIP_EXE']} x {environ['KRITA_INSTALLER']}"
    try:
        subprocess.check_call(commandToRun, stdout=sys.stdout, stderr=sys.stderr, shell=True)
    except subprocess.CalledProcessError:
        warnings.warn(f"ERROR failed to extract installer {environ['KRITA_INSTALLER']}")
        exit(1)
    environ['KRITA_DIR'] = fr"{environ['OUTPUT_DIR']}\installer_content"
    shutil.rmtree(fr"{environ['KRITA_DIR']}\$PLUGINSDIR")
    os.remove(fr"{environ['KRITA_DIR']}\uninstall.exe.nsis")
    os.remove(fr"{environ['KRITA_DIR']}\uninstall.exe")


    print("=== Step 0 done. ===")


if environ.get('KRITA_SHELLEX'):

    print("\n=== Step 0: Copy files for shell extension")

    shellex = fr"{environ['KRITA_DIR']}\shellex"
    try:
        os.mkdir(shellex)
    except:
        warnings.warn("ERROR mkdir shellex failed")
        exit(1)
    try:
        shutil.copy(fr"{environ['KRITA_SHELLEX']}\krita.ico", shellex)
    except:
        warnings.warn("ERROR copying krita.ico failed")
        exit(1)
    try:
        shutil.copy(fr"{environ['KRITA_SHELLEX']}\kritafile.ico", shellex)
    except:
        warnings.warn("ERROR copying kritafile.ico failed")
        exit(1)
    try:
        shutil.copy(fr"{environ['KRITA_SHELLEX']}\kritashellex32.dll", shellex)
    except:
        warnings.warn("ERROR copying kritashellex32.dll failed")
        exit(1)
    try:
        shutil.copy(fr"{environ['KRITA_SHELLEX']}\kritashellex64.dll", shellex)
    except:
        warnings.warn("ERROR copying kritashellex64.dll failed")
        exit(1)
    # Optional files:
    try:
        shutil.copy(fr"{environ['KRITA_SHELLEX']}\kritashellex32.pdb", shellex)
    except:
        pass
    try:
        shutil.copy(fr"{environ['KRITA_SHELLEX']}\kritashellex64.pdb", shellex)
    except:
        pass

    print("=== Step 0 done. ===")

# Sanity checks:
if not os.path.isfile(fr"{environ['KRITA_DIR']}\bin\krita.exe"):
    warnings.warn(fr'ERROR: KRITA_DIR is set to "{environ["KRITA_DIR"]}" but {environ["KRITA_DIR"]}\bin\krita.exe" does not exist!')
    exit(1)
if not os.path.isfile(fr"{environ['KRITA_DIR']}\shellex\kritashellex64.dll"):
    warnings.warn(fr'ERROR: "{environ["KRITA_DIR"]}\shellex\kritashellex64.dll" does not exist!')
    exit(1)
if os.path.isfile(fr"{environ['KRITA_DIR']}\bin\.debug"):
    warnings.warn("ERROR: Package dir seems to contain debug symbols [gcc/mingw].")
    exit(1)
if os.path.isfile(fr"{environ['KRITA_DIR']}\bin\*.pdb"):
    warnings.warn("ERROR: Package dir seems to contain debug symbols [msvc].")
    exit(1)
if os.path.isdir(fr"{environ['KRITA_DIR']}\$PLUGINSDIR"):
    warnings.warn("")
    exit(1)
if os.path.isfile(fr"{environ['KRITA_DIR']}\uninstall.exe.nsis"):
    warnings.warn('ERROR: You did not remove "uninstall.exe.nsis."')
    exit(1)
if os.path.isfile(fr"{environ['KRITA_DIR']}\uninstall.exe*"):
    warnings.warn('ERROR: You did not remove "uninstall.exe*".')
    exit(1)


print("\n=== Step 1: Generate resources.pri ===")

commandToRun = r'"%MAKEPRI%" new /pr "%SCRIPT_DIR%pkg" /mn "%SCRIPT_DIR%manifest.xml" /cf "%SCRIPT_DIR%priconfig.xml" /o /of "%OUTPUT_DIR%\resources.pri"'
try:
    subprocess.check_call(commandToRun, stdout=sys.stdout, stderr=sys.stderr, shell=True)
except subprocess.CalledProcessError:
    warnings.warn("ERROR running makepri")
    exit(1)

print("=== Step 1 done. ===")


print("\n=== Step 2: Generate file mapping list ===")

environ['ASSETS_DIR'] = fr"{environ['SCRIPT_DIR']}pkg\Assets"
environ['MAPPING_OUT'] = fr"{environ['OUTPUT_DIR']}\mapping.txt"

OUT_TEMP = get_temp_file()
print(f"Writing list to temporary file {OUT_TEMP}")

print("[Files]", file=OUT_TEMP)
print(fr'"{environ["SCRIPT_DIR"]}manifest.xml" "AppxManifest.xml"', file=OUT_TEMP)
print(fr'"{environ["OUTPUT_DIR"]}\resources.pri" "Resources.pri"', file=OUT_TEMP)

# Krita application files:
for root, dirs, files in os.walk(environ['KRITA_DIR']):
    for file in files:
        f = os.path.join(root, file)
        print(fr'"{f}" "krita\{os.path.relpath(f, environ["KRITA_DIR"])}"', file=OUT_TEMP)

# Assets:
for root, dirs, files in os.walk(environ['ASSETS_DIR']):
    for file in files:
        f = os.path.join(root, file)
        print(fr'"{f}" "Assets\{os.path.relpath(f, environ["ASSETS_DIR"])}"', file=OUT_TEMP)

shutil.copy(OUT_TEMP, environ['MAPPING_OUT'])
os.remove(OUT_TEMP)

print(f'Written mapping file to "{environ["MAPPING_OUT"]}"')
print("=== Step 2 done. ===")


print("\n=== Step 3: Make MSIX with makeappx.exe ===")

r"""
(this is a comment block...)

For reference, the MSIX Packaging tool uses the following command arguments:
    pack /v /o /l /nv /nfv /f "%UserProfile%\AppData\Local\Packages\Microsoft.MsixPackagingTool_8wekyb3d8bbwe\LocalState\DiagOutputDir\Logs\wox1ifkc.h0i.txt" /p "D:\dev\krita\msix\Krita-testing_4.3.0.0_x64__svcxxs8w6n55m.msix"

The arguments stands for:
    pack: Creates a package.
    /v: Enable verbose logging output to the console.
    /o: Overwrites the output file if it exists. If you don't specify this option or the /no option, the user is asked whether they want to overwrite the file.
    /l: Used for localized packages. The default validation trips on localized packages. This options disables only that specific validation, without requiring that all validation be disabled.
    /nv: Skips semantic validation. If you don't specify this option, the tool performs a full validation of the package.
    /nfv: ???
    /f <mapping file>: Specifies the mapping file.
    /p <output package name>: Specifies the app package or bundle.
"""

commandToRun = r'"%MAKEAPPX%" pack /v /f "%OUTPUT_DIR%\mapping.txt" /p "%OUTPUT_DIR%\krita.msix" /o'
try:
    subprocess.check_call(commandToRun, stdout=sys.stdout, stderr=sys.stderr, shell=True)
except subprocess.CalledProcessError:
    warnings.warn("ERROR running makeappx")
    exit(1)
    
print(f"\nMSIX generated as {environ['OUTPUT_DIR']}\\krita.msix")

if environ.get['SIGNTOOL_SIGN_FLAGS']:
    print("Signing MSIX...")
    commandToRun = r'"%SIGNTOOL%" sign %SIGNTOOL_SIGN_FLAGS% /fd sha256 "%OUTPUT_DIR%\krita.msix"'
try:
    subprocess.check_call(commandToRun, stdout=sys.stdout, stderr=sys.stderr, shell=True)
except subprocess.CalledProcessError:
    warnings.warn("ERROR running signtool\n" +
                  "If you need to specify a PFX keyfile and its password, run:\n" +
                 r'    set SIGNTOOL_SIGN_FLAGS=/f "absolute_path_to_keyfile.pfx" /p password')
    exit(1)


print("=== Step 3 done. ===")

print("*** Script completed ***")
