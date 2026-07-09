import random

first_names = ["Aarav", "Vihaan", "Advik", "Kabir", "Ishaan", "Aryan", "Dhruv", "Ayaan", "Krishna", "Arjun", "Aditya", "Rudra", "Rishi", "Vivaan", "Reyansh", "Ananya", "Diya", "Riya", "Kavya", "Neha", "Zara", "Mira", "Ishita", "Sanya", "Pooja", "Rahul", "Vikram", "Rohan", "Siddharth", "Karan"]
last_names = ["Sharma", "Patel", "Singh", "Kumar", "Gupta", "Verma", "Reddy", "Jain", "Bose", "Das", "Rao", "Nair", "Mehta"]

employees = []
# Ensure unique names because the parser links by name
employees.append(("Aarav_CEO", "CEO", "ROOT"))

vps = []
for i in range(1, 6):
    name = f"{random.choice(first_names)}_{random.choice(last_names)}_VP{i}"
    employees.append((name, f"VP", "Aarav_CEO"))
    vps.append(name)

directors = []
for i in range(6, 26):
    name = f"{random.choice(first_names)}_{random.choice(last_names)}_Dir{i}"
    manager = random.choice(vps)
    employees.append((name, f"Director", manager))
    directors.append(name)

managers = []
for i in range(26, 150):
    name = f"{random.choice(first_names)}_{random.choice(last_names)}_Mgr{i}"
    manager = random.choice(directors)
    employees.append((name, f"Manager", manager))
    managers.append(name)

for i in range(150, 1000):
    name = f"{random.choice(first_names)}_{random.choice(last_names)}_Emp{i}"
    manager = random.choice(managers)
    employees.append((name, "Engineer", manager))

with open("employees_1000.csv", "w") as f:
    for e in employees:
        f.write(f"{e[0]},{e[1]},{e[2]}\n")
