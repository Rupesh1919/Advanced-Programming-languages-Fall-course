import random

DAYS = ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"]
SHIFTS = ["morning", "afternoon", "evening"]
MIN_EMPLOYEES_PER_SHIFT = 2
MAX_DAYS_PER_EMPLOYEE = 5
MAX_EMPLOYEES_PER_SHIFT = 3

def get_employee_data():
    employees = {}
    print("\nEMPLOYEE WEEKLY SCHEDULER")
    print("=" * 60)
    print("Enter employee names and preferred shifts.")
    print("Valid shifts: morning, afternoon, evening")
    print("=" * 60)
    while True:
        try:
            number_of_employees = int(input("\nHow many employees will be scheduled? "))
            if number_of_employees <= 0:
                print("Please enter a number greater than 0.")
                continue
            break
        except ValueError:
            print("Invalid input. Please enter a whole number.")
    if number_of_employees < 9:
        print("\nWARNING:")
        print("At least 9 employees are normally required to guarantee")
        print("2 employees per shift for all 7 days while limiting")
        print("each employee to a maximum of 5 working days.\n")
    for employee_number in range(1, number_of_employees + 1):
        while True:
            name = input(f"\nEnter employee #{employee_number} name: ").strip()
            if name == "":
                print("Employee name cannot be empty.")
            elif name in employees:
                print("That employee name already exists.")
            else:
                break
        employees[name] = {"preferences": {}, "days_worked": 0}
        print(f"\nEnter {name}'s preferred shift for each day.")
        for day in DAYS:
            while True:
                preference = input(f"{day} preference (morning/afternoon/evening): ").strip().lower()
                if preference in SHIFTS:
                    employees[name]["preferences"][day] = preference
                    break
                print("Invalid shift. Enter morning, afternoon, or evening.")
    return employees

def create_empty_schedule():
    schedule = {}
    for day in DAYS:
        schedule[day] = {}
        for shift in SHIFTS:
            schedule[day][shift] = []
    return schedule

def employee_works_on_day(schedule, employee, day):
    for shift in SHIFTS:
        if employee in schedule[day][shift]:
            return True
    return False

def assign_employee(schedule, employees, employee, day, shift):
    if employee_works_on_day(schedule, employee, day):
        return False
    if employees[employee]["days_worked"] >= MAX_DAYS_PER_EMPLOYEE:
        return False
    if len(schedule[day][shift]) >= MAX_EMPLOYEES_PER_SHIFT:
        return False
    schedule[day][shift].append(employee)
    employees[employee]["days_worked"] += 1
    return True

def find_alternative_shift(schedule, day, preferred_shift):
    for shift in SHIFTS:
        if shift != preferred_shift and len(schedule[day][shift]) < MAX_EMPLOYEES_PER_SHIFT:
            return shift
    return None

def assign_on_next_day(schedule, employees, employee, current_day_index):
    next_day_index = current_day_index + 1
    if next_day_index >= len(DAYS):
        return False
    next_day = DAYS[next_day_index]
    if employee_works_on_day(schedule, employee, next_day):
        return False
    preferred_shift = employees[employee]["preferences"][next_day]
    if assign_employee(schedule, employees, employee, next_day, preferred_shift):
        return True
    alternative = find_alternative_shift(schedule, next_day, preferred_shift)
    if alternative is not None:
        if assign_employee(schedule, employees, employee, next_day, alternative):
            return True
    return False

def assign_preferred_shifts(schedule, employees):
    for day in DAYS:
        for shift in SHIFTS:
            while len(schedule[day][shift]) < MIN_EMPLOYEES_PER_SHIFT:
                available_employees = [
                    employee for employee in employees
                    if employees[employee]["days_worked"] < MAX_DAYS_PER_EMPLOYEE
                    and not employee_works_on_day(schedule, employee, day)
                ]
                available_employees.sort(key=lambda employee: (
                    employees[employee]["preferences"][day] != shift,
                    employees[employee]["days_worked"]
                ))
                if not available_employees:
                    break
                assign_employee(schedule, employees, available_employees[0], day, shift)

def fill_understaffed_shifts(schedule, employees):
    for day in DAYS:
        for shift in SHIFTS:
            while len(schedule[day][shift]) < MIN_EMPLOYEES_PER_SHIFT:
                available_employees = []
                for employee in employees:
                    if employees[employee]["days_worked"] < MAX_DAYS_PER_EMPLOYEE and not employee_works_on_day(schedule, employee, day):
                        available_employees.append(employee)
                if len(available_employees) == 0:
                    print(f"\nWARNING: Unable to completely staff {day} {shift} shift.")
                    break
                preferred_candidates = []
                for employee in available_employees:
                    if employees[employee]["preferences"][day] == shift:
                        preferred_candidates.append(employee)
                if preferred_candidates:
                    selected_employee = random.choice(preferred_candidates)
                else:
                    selected_employee = random.choice(available_employees)
                schedule[day][shift].append(selected_employee)
                employees[selected_employee]["days_worked"] += 1

def validate_schedule(schedule, employees):
    errors = []
    for day in DAYS:
        employee_assignments = {}
        for shift in SHIFTS:
            if len(schedule[day][shift]) < MIN_EMPLOYEES_PER_SHIFT:
                errors.append(f"{day} {shift} has fewer than {MIN_EMPLOYEES_PER_SHIFT} employees.")
            for employee in schedule[day][shift]:
                if employee not in employee_assignments:
                    employee_assignments[employee] = 0
                employee_assignments[employee] += 1
        for employee, count in employee_assignments.items():
            if count > 1:
                errors.append(f"{employee} works more than one shift on {day}.")
    for employee in employees:
        actual_days_worked = 0
        for day in DAYS:
            if employee_works_on_day(schedule, employee, day):
                actual_days_worked += 1
        if actual_days_worked > MAX_DAYS_PER_EMPLOYEE:
            errors.append(f"{employee} works {actual_days_worked} days, which exceeds the maximum of {MAX_DAYS_PER_EMPLOYEE}.")
    return errors

def display_schedule(schedule, employees):
    print("\n")
    print("=" * 70)
    print("FINAL EMPLOYEE WEEKLY SCHEDULE")
    print("=" * 70)
    for day in DAYS:
        print(f"\n{day.upper()}")
        print("-" * 70)
        for shift in SHIFTS:
            workers = schedule[day][shift]
            employee_list = ", ".join(workers) if workers else "No employees assigned"
            print(f"{shift.capitalize():12}: {employee_list}")
    print("\n")
    print("=" * 70)
    print("EMPLOYEE WEEKLY WORKLOAD")
    print("=" * 70)
    for employee in employees:
        print(f"{employee:20} : {employees[employee]['days_worked']} day(s)")

def display_preference_results(schedule, employees):
    print("\n")
    print("=" * 70)
    print("PREFERENCE SATISFACTION")
    print("=" * 70)
    for employee in employees:
        preferred_assignments = 0
        total_assignments = 0
        for day in DAYS:
            for shift in SHIFTS:
                if employee in schedule[day][shift]:
                    total_assignments += 1
                    if employees[employee]["preferences"][day] == shift:
                        preferred_assignments += 1
        percentage = (preferred_assignments / total_assignments) * 100 if total_assignments > 0 else 0
        print(f"{employee:20}: {preferred_assignments}/{total_assignments} preferred shifts ({percentage:.1f}%)")

def main():
    employees = get_employee_data()
    schedule = create_empty_schedule()
    assign_preferred_shifts(schedule, employees)
    fill_understaffed_shifts(schedule, employees)
    display_schedule(schedule, employees)
    display_preference_results(schedule, employees)
    errors = validate_schedule(schedule, employees)
    print("\n")
    print("=" * 70)
    print("SCHEDULE VALIDATION")
    print("=" * 70)
    if len(errors) == 0:
        print("Schedule successfully satisfies all required constraints.")
    else:
        print("The following scheduling issues were detected:")
        for error in errors:
            print("-", error)

if __name__ == "__main__":
    main()