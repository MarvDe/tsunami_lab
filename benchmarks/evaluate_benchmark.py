import pandas as pd
import numpy as np

# CSV-Dateien einlesen
snapshot12 = pd.read_csv("snapshot_12.8006880.csv")
snapshot17 = pd.read_csv("snapshot_17.6006550.csv")
snapshot22 = pd.read_csv("snapshot_22.4005990.csv")

snapshot12["x"] -= 50
snapshot17["x"] -= 50
snapshot22["x"] -= 50


snapshot12 = snapshot12[snapshot12["x"] >= 0].reset_index(drop=True)
snapshot17 = snapshot17[snapshot17["x"] >= 0].reset_index(drop=True)
snapshot22 = snapshot22[snapshot22["x"] >= 0].reset_index(drop=True)


canonical12 = pd.read_csv("benchmarks/nthmp_bp01/canonical_profiles_40.csv").dropna(subset=["water_level"])
canonical17 = pd.read_csv("benchmarks/nthmp_bp01/canonical_profiles_55.csv").dropna(subset=["water_level"])
canonical22 = pd.read_csv("benchmarks/nthmp_bp01/canonical_profiles_70.csv").dropna(subset=["water_level"])


canonical12_interp = np.interp(
    snapshot12["x"],
    canonical12["x_d"],
    canonical12["water_level"]
)
canonical17_interp = np.interp(
    snapshot17["x"],
    canonical17["x_d"],
    canonical17["water_level"]
)
canonical22_interp = np.interp(
    snapshot22["x"],
    canonical22["x_d"],
    canonical22["water_level"]
)


diff12 = snapshot12["height"] + snapshot12["bathymetry"] - canonical12_interp
diff17 = snapshot17["height"] + snapshot17["bathymetry"] - canonical17_interp
diff22 = snapshot22["height"] + snapshot22["bathymetry"] - canonical22_interp


file_diff_12 = pd.DataFrame({
    "x": snapshot12["x"],
    "water_level": diff12
})
file_diff_17 = pd.DataFrame({
    "x": snapshot17["x"],
    "water_level": diff17
})
file_diff_22 = pd.DataFrame({
    "x": snapshot22["x"],
    "water_level": diff22
})


file_diff_12.to_csv("file_diff_12.csv", index=False)
file_diff_17.to_csv("file_diff_17.csv", index=False)
file_diff_22.to_csv("file_diff_22.csv", index=False)



print("Neue CSV wurde erstellt: file_diff_12.csv, file_diff_17.csv, file_diff_22.csv")