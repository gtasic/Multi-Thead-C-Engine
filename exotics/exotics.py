import matplotlib.pyplot as plt 
import pandas as pd
import numpy as np 


df = pd.read_csv("simulation_exotics.csv")

plt.figure(figsize=(10,6))
plt.title("Simulation Monte Carlo : Mouvement Brownien Géométrique")
plt.xlabel("Temps en jours")
plt.ylabel("Prix en euros")

for path_id, group in df.groupby(["PathId"]) : 
    plt.plot(group["Time"], group["Price"], linewidth = 1, alpha = 0.6 )

payoff = 0
for path_id, group in df.groupby(["PathId"]) : 
    payoff += group["Price"].iloc[-1]
print(len(df["PathId"].unique()))
print("Payoff de la barrière :", payoff/len(df["PathId"].unique()))

plt.axhline(y=100, color = "red", linestyle = "--", label = "Prix départ")
plt.savefig("monte_carlo_simulation.png", dpi=300)
plt.show()

N = [50, 100, 200, 500,1000, 5000, 10000, 50000,100000]
prix = [8.75673,5.2115,4.55594,5.31809, 5.95457, 5.72881, 5.60243, 5.78756, 5.73663]

plt.figure(figsize=(10,6))
plt.title("Convergence du prix de la barrière en fonction du nombre de paths simulés")
plt.xlabel("Nombre de paths simulés")
plt.ylabel("Prix de la barrière en euros")
plt.plot(N, prix, marker='o')
plt.xscale('log')
plt.savefig("convergence_prix_barriere.png", dpi=300)



np.random.seed(42)
true_price = 5.74
N_TOTAL = 100000
payoffs = np.random.exponential(scale=true_price, size=N_TOTAL) 

n_steps = [50,100,200,500,1000, 5000, 10000, 50000, 100000]
n_trials = 50 

plt.figure(figsize=(12, 7))

means = []
stds = []

for n in n_steps:
    prices_for_n = []
    for _ in range(n_trials):
        sample = np.random.choice(payoffs, size=n, replace=True)
        price_estimate = np.mean(sample) * np.exp(-0.05 * 1.0) # Discounting
        prices_for_n.append(price_estimate)
        
        plt.scatter(n, price_estimate, color='blue', alpha=0.15, s=10)
    
    means.append(np.mean(prices_for_n))
    stds.append(np.std(prices_for_n))

means = np.array(means)
stds = np.array(stds)

plt.plot(n_steps, means, color='red', linestyle='--', label='Prix Moyen Estimé')

plt.plot(n_steps, means + 2*stds, color='green', linewidth=1, alpha=0.5, label='Intervalle de Confiance 95%')
plt.plot(n_steps, means - 2*stds, color='green', linewidth=1, alpha=0.5)
plt.fill_between(n_steps, means - 2*stds, means + 2*stds, color='green', alpha=0.1)

plt.axhline(y=true_price * np.exp(-0.05), color='black', linestyle='-', linewidth=1, label='Prix de référence')
plt.xscale('log') 
plt.title("Convergence de Monte Carlo : L'Entonnoir de Précision", fontsize=16)
plt.xlabel("Nombre de Simulations (N) - Échelle Log", fontsize=12)
plt.ylabel("Prix Estimé de l'Option ($)", fontsize=12)
plt.legend()
plt.grid(True, which="both", linestyle='--', linewidth=0.5)

plt.show()
plt.savefig("convergence_barrier.png", dpi=300)