import matplotlib.pyplot as plt 
import pandas as pd 
import seaborn as sns

df_mc = pd.read_csv("../utils/simulation_mc.csv")
df_heston = pd.read_csv("../utils/simulation_heston.csv")

final_mc = df_mc.groupby("PathId")["Price"].last()
final_heston = df_heston.groupby("PathId")["Price"].last()

sns.set_theme(style="whitegrid")
plt.figure(figsize=(12, 7))

sns.histplot(final_mc, color="skyblue", label="Black-Scholes (GBM)", kde=True, stat="density", bins=100, alpha=0.4, element="step")
sns.histplot(final_heston, color="orange", label="Heston Model", kde=True, stat="density", bins=100, alpha=0.4, element="step")

plt.title("Analyse des Distributions : Impact de la Volatilité Stochastique", fontsize=16)
plt.xlabel("Prix Final ($)")
plt.axvline(final_heston.mean(), color="red", linestyle="--", alpha=0.7, label="Mean Heston")
plt.axvline(final_mc.mean(), color="blue", linestyle="--", alpha=0.7, label="Mean GBM")
plt.ylabel("Densité")
plt.legend()
plt.savefig("comparaison_mc_heston.png", dpi=300)
plt.show()