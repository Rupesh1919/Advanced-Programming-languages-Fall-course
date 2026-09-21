# Assignment 4: Employee Weekly Scheduler

## Overview

This assignment implements an employee scheduling application in two different programming languages:

- **Python**: `python code.py`
- **JavaScript**: `javascript code.js`

Both programs demonstrate conditionals, `for` and `while` loops, validation, branching, collections, helper functions, and random selection. The application schedules employees across seven days and three shifts: morning, afternoon, and evening.

## Assignment Requirements Implemented

### 1. Input and Storage

The program collects:

- The number of employees
- Each employee's name
- Each employee's preferred shift for every day of the week

Employee data is stored in a dictionary/object. Each employee record contains daily preferences and the number of days already assigned.

### 2. Scheduling Logic

The scheduler applies these rules:

- Monday through Sunday are supported.
- Each day has morning, afternoon, and evening shifts.
- Every shift requires at least two employees.
- Each employee can work no more than one shift per day.
- Each employee can work a maximum of five days per week.
- A shift can contain no more than three employees in this implementation.
- Preferred employees are selected first when possible.
- If a shift still needs employees, an available employee is selected randomly.
- If the employee pool is too small to satisfy all requirements, the program prints a warning instead of exceeding the daily or weekly limits.

At least nine employees are normally needed to provide two employees for each of the 21 weekly shifts while keeping every employee at or below five working days.

### 3. Shift Conflicts

The scheduler checks whether an employee is already working that day before assigning another shift. It also checks whether the employee has reached the five-day weekly limit and whether the target shift has reached its capacity. These checks prevent duplicate daily assignments and invalid weekly workloads.

### 4. Output

After scheduling, the program displays:

- The final schedule for every day and shift
- Each employee's weekly workload
- Preference satisfaction for each employee
- Schedule validation results

Sample outputs are included in:

- `python-output.txt`
- `javascript-output.txt`

### 5. Bonus: Preferences

The programs support the preference-related bonus by recording one preferred shift for each employee on each day. The scheduler attempts to honor these preferences first and reports the percentage of assigned shifts that matched each employee's preference.

## Requirements

### Python

- Python 3.8 or newer
- No external packages are required

Check the installation:

```bash
python3 --version
```

### JavaScript

- Node.js 14 or newer
- `prompt-sync` for interactive terminal input

Check the installation:

```bash
node --version
npm --version
```

Install the JavaScript dependency from the `Assignment 4 code files` directory:

```bash
npm install prompt-sync
```

## How to Run

Open a terminal in this directory:

```bash
cd "Assignment 4 code files"
```

Run the Python version:

```bash
python3 "python code.py"
```

Run the JavaScript version:

```bash
node "javascript code.js"
```

For either program, enter a positive number of employees, then enter a unique name and one of these valid preferences for each day:

```text
morning
afternoon
evening
```

The JavaScript program can also read redirected input, which is useful for repeatable testing:

```bash
node "javascript code.js" < input.txt
```

## Control Structures Demonstrated

- **Conditionals**: input validation, capacity checks, weekly-limit checks, and validation error reporting
- **Loops**: employee input loops, seven-day loops, shift loops, and repeated staffing loops
- **Branching**: preferred-candidate selection versus random fallback selection, valid versus invalid input paths, and successful versus failed validation
- **Collections**: Python dictionaries/lists and JavaScript objects/arrays for employee records and schedules
- **Random selection**: fallback staffing chooses randomly from eligible employees when preferred candidates are unavailable

## Example Result

The included sample runs use nine employees. The schedule validation reports:

```text
Schedule successfully satisfies all required constraints.
```

The sample output also shows two employees assigned to every shift, no employee assigned more than one shift on a day, and no employee assigned more than five days in the week.

Because fallback staffing uses random selection, schedules may differ between runs while still following the validation rules when enough employees are available.

## Submission Checklist

- [x] Source code in two distinct programming languages
- [x] Employee input and shift preference storage
- [x] Seven-day scheduling logic
- [x] Minimum staffing requirement of two employees per shift
- [x] Maximum of one shift per employee per day
- [x] Maximum of five working days per employee per week
- [x] Conflict and capacity checks
- [x] Readable final schedule output
- [x] Preference satisfaction output
- [x] Schedule validation output
- [ ] Add screenshot(s) of the final schedule output to the submission document
- [ ] Include the public GitHub repository link in the submission document

## Files

| File | Description |
| --- | --- |
| `python code.py` | Python implementation |
| `javascript code.js` | JavaScript implementation |
| `python-output.txt` | Example Python execution output |
| `javascript-output.txt` | Example JavaScript execution output |
| `README.md` | Assignment description and run instructions |
