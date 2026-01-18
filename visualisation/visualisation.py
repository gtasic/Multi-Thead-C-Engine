import matplotlib.pyplot as plt 
import pandas as pd



df = pd.read_csv("../utils/simulation_data.csv")

plt.figure(figsize=(10,6))
plt.title("Simulation Monte Carlo : Mouvement Brownien Géométrique")
plt.xlabel("Temps en Jours")
plt.ylabel("Prix en euros")

for path_id, group in df.groupby(["PathId"]) : 
    plt.plot(group["Time"], group["Price"], linewidth = 1, alpha = 0.6 )

plt.axhline(y=100, color = "red", linestyle = "--", label = "Prix départ")
plt.savefig("monte_carlo_simulation.png", dpi=300)
plt.show()