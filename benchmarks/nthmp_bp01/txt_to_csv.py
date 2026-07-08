from pathlib import Path
import csv
import argparse


def canonical_txt_to_two_csvs(input_file: str):
    input_path = Path(input_file)

    output_025 = input_path.with_name("canonical_ts_xd_0_25.csv")
    output_995 = input_path.with_name("canonical_ts_xd_9_95.csv")

    rows_025 = []
    rows_995 = []
    data_started = False

    with input_path.open("r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()

            if not line:
                continue

            # Tabellenkopf erkennen
            if line.startswith("t/tau"):
                data_started = True
                continue

            if data_started:
                parts = line.split()

                # Erwartetes Format:
                # t/tau  value_xd_0.25  t/tau  value_xd_9.95
                if len(parts) == 4:
                    rows_025.append([parts[0], parts[1]])
                    rows_995.append([parts[2], parts[3]])
                if len(parts) == 2:
                    rows_025.append([parts[0], parts[1]])

    with output_025.open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["t_tau", "water_level"])
        writer.writerows(rows_025)

    with output_995.open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["t_tau", "water_level"])
        writer.writerows(rows_995)

    print(f"CSV erstellt: {output_025}")
    print(f"CSV erstellt: {output_995}")

def canonical_profiles_to_csvs(input_file: str):
    input_path = Path(input_file)

    output_35 = input_path.with_name("canonical_profiles_35.csv")
    output_40 = input_path.with_name("canonical_profiles_40.csv")
    output_45 = input_path.with_name("canonical_profiles_45.csv")
    output_50 = input_path.with_name("canonical_profiles_50.csv")
    output_55 = input_path.with_name("canonical_profiles_55.csv")
    output_60 = input_path.with_name("canonical_profiles_60.csv")
    output_65 = input_path.with_name("canonical_profiles_65.csv")
    output_70 = input_path.with_name("canonical_profiles_70.csv")
    
    rows_35 = []
    rows_40 = []
    rows_45 = []
    rows_50 = []
    rows_55 = []
    rows_60 = []
    rows_65 = []
    rows_70 = []
    
    data_started = False

    with input_path.open("r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()

            if not line:
                continue

            # Tabellenkopf erkennen
            if line.startswith("x/d"):
                data_started = True
                continue

            if data_started:
                parts = line.split()

                # Erwartetes Format:
                # x/d t/tau=35 t/tau=40	t/tau=45 t/tau=50 t/tau=55 t/tau=60	t/tau=65 t/tau=70

                if len(parts) == 9:
                        rows_35.append([parts[0], parts[1]])
                        rows_40.append([parts[0], parts[2]])
                        rows_45.append([parts[0], parts[3]])
                        rows_50.append([parts[0], parts[4]])
                        rows_55.append([parts[0], parts[5]])
                        rows_60.append([parts[0], parts[6]])
                        rows_65.append([parts[0], parts[7]])
                        rows_70.append([parts[0], parts[8]])

    with output_35.open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["x_d", "water_level"])
        writer.writerows(rows_35)
    
    with output_40.open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["x_d", "water_level"])
        writer.writerows(rows_40)

    with output_45.open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["x_d", "water_level"])
        writer.writerows(rows_45)
    
    with output_50.open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["x_d", "water_level"])
        writer.writerows(rows_50)
    

    with output_55.open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["x_d", "water_level"])
        writer.writerows(rows_55)

    with output_60.open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["x_d", "water_level"])
        writer.writerows(rows_60)

    with output_65.open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["x_d", "water_level"])
        writer.writerows(rows_65)

    with output_70.open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["x_d", "water_level"])
        writer.writerows(rows_70)

    print(f"CSV erstellt: {output_35}")
    print(f"CSV erstellt: {output_40}")
    print(f"CSV erstellt: {output_45}")
    print(f"CSV erstellt: {output_50}")
    print(f"CSV erstellt: {output_55}")
    print(f"CSV erstellt: {output_60}")
    print(f"CSV erstellt: {output_65}")
    print(f"CSV erstellt: {output_70}")




if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Create two CSV files canonial input files"
    )
    parser.add_argument(
        "input",
        nargs="?",
        default="canonical_ts.txt",
        help="Input TXT file"
    )
    parser.add_argument(
        "ty",
        nargs="?",
        default=0,
        help="type: 0 = canonial_txt, 1 = canonial_profiles"
    )
    args = parser.parse_args()
    if args.ty == 0:
        canonical_txt_to_two_csvs(args.input)

    else:
        canonical_profiles_to_csvs(args.input)

        