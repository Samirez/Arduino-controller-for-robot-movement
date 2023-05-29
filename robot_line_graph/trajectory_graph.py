import matplotlib.pyplot as plt
import pandas as pd
import sqlite3

conn = sqlite3.connect("robDB.db")

sql = """select x, z from robot_movement order by name asc"""

data = pd.read_sql(sql, conn)

plt.plot(data.x, data.z, label="robot movement")
plt.legend()
plt.title("robot movement")
plt.show()
