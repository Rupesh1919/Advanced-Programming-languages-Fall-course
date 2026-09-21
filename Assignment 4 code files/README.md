# Assignment 4: Employee Scheduler

## What I Made

For this assignment, I made the same employee scheduling program in Python and JavaScript. The program asks for employee names and their preferred shift for each day of the week. It then creates a schedule for the morning, afternoon, and evening shifts.

The program makes sure that:

- Each shift has at least two employees when enough employees are available.
- An employee does not work more than one shift in the same day.
- An employee works no more than five days in one week.
- Employee preferences are used when possible.
- Employees are selected randomly when a shift still needs more people.

At the end, the program prints the weekly schedule, each employee's workload, preference results, and a validation message.

## Languages Used

- Python: `python code.py`
- JavaScript: `javascript code.js`

I used Python dictionaries and lists, JavaScript objects and arrays, loops, conditionals, input validation, and random selection in both versions.

## How to Run the Python Program

Python 3 is required. No extra Python packages are needed.

From the project folder, run:

```bash
python3 "Assignment 4 code files/python code.py"
```

## How to Run the JavaScript Program

Node.js is required. The JavaScript program uses the `prompt-sync` package for keyboard input. Install it once by running this from the project folder:

```bash
npm install prompt-sync
```

Then run:

```bash
node "Assignment 4 code files/javascript code.js"
```

When prompted, enter the number of employees, each employee's name, and a preference for every day. Valid preferences are:

```text
morning
afternoon
evening
```

Nine employees are recommended because the schedule needs 42 total assignments and each employee can work only five days.

## Example Output

The sample output files show a completed schedule:

- `python-output.txt`
- `javascript-output.txt`

Both sample runs show two employees on every shift and end with:

```text
Schedule successfully satisfies all required constraints.
```

The schedule can be different each time because the program randomly chooses employees when it needs extra staffing.

## Files in This Folder

| File | Description |
| --- | --- |
| `python code.py` | Python version of the scheduler |
| `javascript code.js` | JavaScript version of the scheduler |
| `python-output.txt` | Sample Python output |
| `javascript-output.txt` | Sample JavaScript output |
| `README.md` | Project information and run instructions |

## Submission Items

- Source code in Python and JavaScript
- Screenshot of the final schedule output
- Link to the public GitHub repository
