# Commands to Compile and Run `org_chart.c`

Here are the commands you can use in your terminal to compile and run your organization chart C program.

## Compilation

You can compile the code using `gcc`. Open your terminal (or command prompt) and run:

```bash
gcc org_chart.c -o org_chart.exe
```

This will generate an executable file named `org_chart.exe`.

## Execution

The program expects an input string where each employee is formatted as `Name, Role, ManagerName` and employees are separated by a `|` (pipe) character. Note that the top-level manager should have `ROOT` as their manager name.

You can run the program and pass the input string as a command-line argument:

```bash
.\org_chart.exe "Alice, CEO, ROOT|Bob, VP of Engineering, Alice|Charlie, Senior Dev, Bob|David, Junior Dev, Bob|Eve, VP of Sales, Alice"
```

Alternatively, if you run the program without arguments, it will wait for you to type the input (press Enter when done):

```bash
.\org_chart.exe
```
*(Then type your input string such as `Alice, CEO, ROOT|Bob, VP, Alice` and press Enter)*
