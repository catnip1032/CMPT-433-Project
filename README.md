#Recycler
## Directory Structure
- **src:** Add source (.c) files to this directory (except for test.c; see 
section Tests).
- **include**: Add header files (.h) to this directory.
- **build**: Where all object files (.o) will be placed during building. There 
is no necessity for  anything to be added to this directory. 
This directory SHOULD NOT be committed.
- **test**: Where test.c and other necessary test files should be included.

## Style and Other Guidelines
- Indentation: 2 columns (tab).
- Wrap at 80 characters.
- Allman brackets style:
```
if (condition) 
{
	action
}
```
- Try to abide to other style guidelines as prescribed by the course website.
- Separate logical sections of the code with a section name (e.g. Definitions,
Function Implementations) followed by a newline, two comment bars, a space and
exactly 76 dashes. Example:
```
GPIO Functions
// ----------------------------------------------------------------------------
```
- Everything, except for functions and variables included in the header files,
should be static.
- Use `stdint` types (e.g. `uint_8`, `uint_64`) for integers.
- Use `bool` for boolean literals.
- Use `typedef` for defining structs, enums, and primitive data types that have
a certain context.
- Unless casting a `void *` or among primitive data types, prefer defining a
function that takes in a data type and returns another data type.
- Every module that comprises a functionality of the program should have its
`initiate` and `terminate` functions, even if they don't do anything. This does
not apply for "utility" modules (e.g. modules with "library" functions, such as
math functions, or algorithms that don't comprise part of the program's
behavior).
- Functions that allocate memory should have a comment stating that the caller
must free the memory.
- Function names should start with the module name followed by what it does:
```
GPIO_getGpio(args...)
```
- Global variables must be prefixed with `m_`, arguments must be prefixed with
`_`, and function variables do not require any prefix. Example:
```
static int m_globalVariable = 0;

int foo(char _c) {
    char anotherChar = _c;
    return 1;
  }
```

## Building
**make OR make all:** Compiles the files from `src` including the header files 
in `include`, producing object files to `build` and generating the binary to 
`~/cmpt433/public/myApps`. Creates the aforementioned folder structure if it 
does not exist.

**make test:** Compiles the files from `src` (except for the file `main.c`), 
the file `test/test.c`, including the header files in `include`, producing 
object files to `build` and generating the binary to `~/cmpt433/public/tests`. 
Creates the aforementioned folder structure if it does not exist.

**make clean**: Removes the produced binary, the test binary, and all objects 
in `build`.

## Must do before execution

### Edit the device tree on your BeagleBone Green to include Audio, I2C, and PWM capes:

1. Navigate to 

```
    # cd /boot/
```

2. Create a copy of your ```uEnv.txt``` in case anything goes wrong

```
    # sudo cp /boot/uEnv.txt /boot/uEnv-BeforeAddCapes.txt
```

3. Open the ```uEnv.txt``` file to edit
```
    # sudo nano uEnv.txt
```

4. Add these additional capes under ```###Additional custom capes```
```
    ###Additional custom capes
    uboot_overlay_addr4=/lib/firmware/BB-BONE-AUDI-02-00A0.dtbo
    uboot_overlay_addr5=/lib/firmware/BB-I2C1-00A0.dtbo
    uboot_overlay_addr6=/lib/firmware/BB-PWM0-00A0.dtbo
    uboot_overlay_addr7=/lib/firmware/BB-PWM1-00A0.dtbo
    uboot_overlay_addr3=/lib/firmware/BB-PWM2-00A0.dtbo
```

5. Reboot your BeagleBone Green

## Git Workflow
 - Attempt to limit the scope of your work to a git issue. If no git issue 
exists for the work that you will be doing, create a new git issue for that
task.
 - Isolate your work on a branch that corresponds to its git issue. Try to
abide to the following format: *#-issue-name*, where '#' is the issue number
and "issue-name" is the respective issue name.
 - After work is ready to merge, make a pull request and wait for someone to
 review your changes.
 - Make use of git issues to comment on each other's work as needed.

## Tests
Unitary tests are encouraged to be written in the file `test/test.c`. The code
suggests a layout to be followed for unitary tests. More information can be
found in the file comments.

