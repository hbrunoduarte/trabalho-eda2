import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from scipy.optimize import curve_fit

def log_model(x, a, b):
    return a * np.log(x) + b

datasets = [
    {"file": "avl.csv",      "label": "AVL",               "color": "red"},
    {"file": "rn.csv",       "label": "Rubro-Negra",       "color": "black"},
    {"file": "btree1.csv",   "label": "B-Tree (Grau 1)",   "color": "blue"},
    {"file": "btree5.csv",   "label": "B-Tree (Grau 5)",   "color": "green"},
    {"file": "btree10.csv",  "label": "B-Tree (Grau 10)",  "color": "orange"}
]

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 12))

for ds in datasets:
    try:
        df = pd.read_csv(ds["file"], header=None)
        
        # Parse X
        x_raw = df.iloc[0, :].values
        x_values = np.array([int(val.split('_')[1]) for val in x_raw])
        
        # Prepare Data
        x_full = np.insert(x_values, 0, 0)
        y_inserts = np.insert(df.iloc[1, :].values.astype(float), 0, 0)
        y_deletes = np.insert(df.iloc[2, :].values.astype(float), 0, 0)
        
        # Calculate Derivatives
        dx = np.diff(x_full)
        marginal_inserts = np.diff(y_inserts) / dx
        marginal_deletes = np.diff(y_deletes) / dx
        
        # --- Regression for Insertion ---
        valid_idx = x_values > 0
        x_fit = x_values[valid_idx]
        y_fit_ins = marginal_inserts[valid_idx]
        
        popt_ins, _ = curve_fit(log_model, x_fit, y_fit_ins)
        
        # Plot ONLY Regression Line
        x_line = np.linspace(min(x_fit), max(x_fit), 100)
        ax1.plot(x_line, log_model(x_line, *popt_ins), color=ds["color"], linewidth=2, linestyle='-', label=f'{ds["label"]}')

        # --- Regression for Deletion ---
        y_fit_del = marginal_deletes[::-1][valid_idx] # Invert to match x
        
        popt_del, _ = curve_fit(log_model, x_fit, y_fit_del)
        
        # Plot ONLY Regression Line
        ax2.plot(x_line, log_model(x_line, *popt_del), color=ds["color"], linewidth=2, linestyle='-', label=f'{ds["label"]}')
        
    except Exception as e:
        print(f"Error processing {ds['file']}: {e}")

# Styling
ax1.set_title("Regressão Logarítmica - Custo Marginal de INSERÇÃO", fontsize=14)
ax1.set_ylabel("Iterações", fontsize=12)
ax1.legend(loc='upper left', bbox_to_anchor=(1, 1), fontsize='small')
ax1.grid(True, linestyle='--', alpha=0.7)

ax2.set_title("Regressão Logarítmica - Custo Marginal de REMOÇÃO", fontsize=14)
ax2.set_xlabel("Tamanho da Árvore", fontsize=12)
ax2.set_ylabel("Iterações", fontsize=12)
ax2.legend(loc='upper left', bbox_to_anchor=(1, 1), fontsize='small')
ax2.grid(True, linestyle='--', alpha=0.7)

plt.tight_layout()
plt.show()