let prompt;
if (process.stdin.isTTY) {
prompt = require("prompt-sync")({ sigint: true });
} else {
const inputLines = require("fs").readFileSync(0, "utf8").split(/\r?\n/);
let inputIndex = 0;
prompt = () => inputLines[inputIndex++] || "";
}
const DAYS = ["Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"];
const SHIFTS = ["morning", "afternoon", "evening"];
const MIN_EMPLOYEES_PER_SHIFT = 2;
const MAX_DAYS_PER_EMPLOYEE = 5;
const MAX_EMPLOYEES_PER_SHIFT = 3;
function shuffleArray(array) {
for (let i = array.length - 1; i > 0; i--) {
const j = Math.floor(Math.random() * (i + 1));
const temp = array[i];
array[i] = array[j];
array[j] = temp;
}
return array;
}
function getEmployeeData() {
const employees = {};
console.log("\nEMPLOYEE WEEKLY SCHEDULER");
console.log("=".repeat(60));
let employeeCount;
while (true) {
const value = prompt("\nHow many employees will be scheduled? ");
employeeCount = parseInt(value);
if (!Number.isNaN(employeeCount) && employeeCount > 0) {
break;
}
console.log("Please enter a valid positive whole number.");
}
if (employeeCount < 9) {
console.log("\nWARNING:");
console.log("At least 9 employees are normally required to guarantee");
console.log("2 employees per shift for all 7 days while limiting");
console.log("each employee to a maximum of 5 working days.");
}
for (let employeeNumber = 1; employeeNumber <= employeeCount; employeeNumber++) {
let name;
while (true) {
name = prompt(`\nEnter employee #${employeeNumber} name: `).trim();
if (name === "") {
console.log("Employee name cannot be empty.");
} else if (employees[name]) {
console.log("That employee name already exists.");
} else {
break;
}
}
employees[name] = {
preferences: {},
daysWorked: 0
};
console.log(`\nEnter ${name}'s preferred shift for each day.`);
for (const day of DAYS) {
while (true) {
const preference = prompt(`${day} preference (morning/afternoon/evening): `).trim().toLowerCase();
if (SHIFTS.includes(preference)) {
employees[name].preferences[day] = preference;
break;
}
console.log("Invalid shift. Enter morning, afternoon, or evening.");
}
}
}
return employees;
}
function createEmptySchedule() {
const schedule = {};
for (const day of DAYS) {
schedule[day] = {};
for (const shift of SHIFTS) {
schedule[day][shift] = [];
}
}
return schedule;
}
function employeeWorksOnDay(schedule, employee, day) {
for (const shift of SHIFTS) {
if (schedule[day][shift].includes(employee)) {
return true;
}
}
return false;
}
function assignEmployee(schedule, employees, employee, day, shift) {
if (employeeWorksOnDay(schedule, employee, day)) {
return false;
}
if (employees[employee].daysWorked >= MAX_DAYS_PER_EMPLOYEE) {
return false;
}
if (schedule[day][shift].length >= MAX_EMPLOYEES_PER_SHIFT) {
return false;
}
schedule[day][shift].push(employee);
employees[employee].daysWorked++;
return true;
}
function findAlternativeShift(schedule, day, preferredShift) {
for (const shift of SHIFTS) {
if (shift !== preferredShift && schedule[day][shift].length < MAX_EMPLOYEES_PER_SHIFT) {
return shift;
}
}
return null;
}
function assignOnNextDay(schedule, employees, employee, currentDayIndex) {
const nextDayIndex = currentDayIndex + 1;
if (nextDayIndex >= DAYS.length) {
return false;
}
const nextDay = DAYS[nextDayIndex];
if (employeeWorksOnDay(schedule, employee, nextDay)) {
return false;
}
const preferredShift = employees[employee].preferences[nextDay];
if (assignEmployee(schedule, employees, employee, nextDay, preferredShift)) {
return true;
}
const alternative = findAlternativeShift(schedule, nextDay, preferredShift);
if (alternative !== null) {
if (assignEmployee(schedule, employees, employee, nextDay, alternative)) {
return true;
}
}
return false;
}
function assignPreferredShifts(schedule, employees) {
for (const day of DAYS) {
for (const shift of SHIFTS) {
while (schedule[day][shift].length < MIN_EMPLOYEES_PER_SHIFT) {
const availableEmployees = Object.keys(employees).filter(employee =>
employees[employee].daysWorked < MAX_DAYS_PER_EMPLOYEE &&
!employeeWorksOnDay(schedule, employee, day)
);
availableEmployees.sort((first, second) => {
const firstPreferred = employees[first].preferences[day] === shift ? 0 : 1;
const secondPreferred = employees[second].preferences[day] === shift ? 0 : 1;
return firstPreferred - secondPreferred ||
employees[first].daysWorked - employees[second].daysWorked;
});
if (availableEmployees.length === 0) {
break;
}
assignEmployee(schedule, employees, availableEmployees[0], day, shift);
}
}
}
}
function fillUnderstaffedShifts(schedule, employees) {
for (const day of DAYS) {
for (const shift of SHIFTS) {
while (schedule[day][shift].length < MIN_EMPLOYEES_PER_SHIFT) {
const availableEmployees = [];
for (const employee of Object.keys(employees)) {
const belowWeeklyLimit = employees[employee].daysWorked < MAX_DAYS_PER_EMPLOYEE;
const notWorkingToday = !employeeWorksOnDay(schedule, employee, day);
if (belowWeeklyLimit && notWorkingToday) {
availableEmployees.push(employee);
}
}
if (availableEmployees.length === 0) {
console.log(`\nWARNING: Unable to completely staff ${day} ${shift} shift.`);
break;
}
const preferredCandidates = availableEmployees.filter(
employee => employees[employee].preferences[day] === shift
);
let selectedEmployee;
if (preferredCandidates.length > 0) {
selectedEmployee = preferredCandidates[Math.floor(Math.random() * preferredCandidates.length)];
} else {
selectedEmployee = availableEmployees[Math.floor(Math.random() * availableEmployees.length)];
}
schedule[day][shift].push(selectedEmployee);
employees[selectedEmployee].daysWorked++;
}
}
}
}
function validateSchedule(schedule, employees) {
const errors = [];
for (const day of DAYS) {
const employeeAssignments = {};
for (const shift of SHIFTS) {
if (schedule[day][shift].length < MIN_EMPLOYEES_PER_SHIFT) {
errors.push(`${day} ${shift} has fewer than ${MIN_EMPLOYEES_PER_SHIFT} employees.`);
}
for (const employee of schedule[day][shift]) {
if (!employeeAssignments[employee]) {
employeeAssignments[employee] = 0;
}
employeeAssignments[employee]++;
}
}
for (const employee of Object.keys(employeeAssignments)) {
if (employeeAssignments[employee] > 1) {
errors.push(`${employee} works more than one shift on ${day}.`);
}
}
}
for (const employee of Object.keys(employees)) {
let actualDaysWorked = 0;
for (const day of DAYS) {
if (employeeWorksOnDay(schedule, employee, day)) {
actualDaysWorked++;
}
}
if (actualDaysWorked > MAX_DAYS_PER_EMPLOYEE) {
errors.push(`${employee} works ${actualDaysWorked} days, which exceeds the maximum of ${MAX_DAYS_PER_EMPLOYEE}.`);
}
}
return errors;
}
function displaySchedule(schedule, employees) {
console.log("\n");
console.log("=".repeat(70));
console.log("FINAL EMPLOYEE WEEKLY SCHEDULE");
console.log("=".repeat(70));
for (const day of DAYS) {
console.log(`\n${day.toUpperCase()}`);
console.log("-".repeat(70));
for (const shift of SHIFTS) {
const workers = schedule[day][shift];
const employeeList = workers.length > 0 ? workers.join(", ") : "No employees assigned";
console.log(`${shift.charAt(0).toUpperCase() + shift.slice(1)}: ${employeeList}`);
}
}
console.log("\n");
console.log("=".repeat(70));
console.log("EMPLOYEE WEEKLY WORKLOAD");
console.log("=".repeat(70));
for (const employee of Object.keys(employees)) {
console.log(`${employee}: ${employees[employee].daysWorked} day(s)`);
}
}
function displayPreferenceResults(schedule, employees) {
console.log("\n");
console.log("=".repeat(70));
console.log("PREFERENCE SATISFACTION");
console.log("=".repeat(70));
for (const employee of Object.keys(employees)) {
let preferredAssignments = 0;
let totalAssignments = 0;
for (const day of DAYS) {
for (const shift of SHIFTS) {
if (schedule[day][shift].includes(employee)) {
totalAssignments++;
if (employees[employee].preferences[day] === shift) {
preferredAssignments++;
}
}
}
}
let percentage = 0;
if (totalAssignments > 0) {
percentage = (preferredAssignments / totalAssignments) * 100;
}
console.log(`${employee}: ${preferredAssignments}/${totalAssignments} preferred shifts (${percentage.toFixed(1)}%)`);
}
}
function main() {
const employees = getEmployeeData();
const schedule = createEmptySchedule();
assignPreferredShifts(schedule, employees);
fillUnderstaffedShifts(schedule, employees);
displaySchedule(schedule, employees);
displayPreferenceResults(schedule, employees);
const errors = validateSchedule(schedule, employees);
console.log("\n");
console.log("=".repeat(70));
console.log("SCHEDULE VALIDATION");
console.log("=".repeat(70));
if (errors.length === 0) {
console.log("Schedule successfully satisfies all required constraints.");
} else {
console.log("The following scheduling issues were detected:");
for (const error of errors) {
console.log("- " + error);
}
}
}
main();