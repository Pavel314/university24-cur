from pathlib import Path
import sys
import re

def select_csv(title):
    from tkinter import Tk
    from tkinter.filedialog import askopenfilename
    root = Tk()
    root.withdraw()
    root.attributes("-topmost", True)
    file_path = askopenfilename(
        title=title,
        filetypes=[("CSV Files", "*.csv"), ("All Files", "*.*")],
    )
    root.destroy()
    return Path(file_path) if file_path else None

def extract_params(text: str):
    match = re.search(r'\[(.*?)\]', text)        
    return match.group(1) if match else text

def init_fig(file_path: Path):
    import pandas as pd
    import matplotlib.pyplot as plt
    
    data = pd.read_csv(file_path)
    title = f"Phase Portrait\n{extract_params(file_path.stem)}"
    plt.style.use('seaborn-v0_8-whitegrid')
    fig, ax = plt.subplots(figsize=(8, 8))
    ax.plot(data['X'], data['Y'], linewidth=1.5, label='Trajectory')
    ax.plot(data['X'].iloc[0], data['Y'].iloc[0], 'ro', label='Start')
    ax.set_title(title, fontweight='bold', pad=12)
    ax.set_xlabel("X-axis")
    ax.set_ylabel("Y-axis")
    ax.set_aspect('equal', 'box')
    ax.legend()
    plt.tight_layout()
    return fig

def run_gui():
    file_path = select_csv("Select the CSV file with points")
    if not file_path: print("File not found"); return
    
    import matplotlib.pyplot as plt
    fig = init_fig(file_path)
    fig.canvas.manager.set_window_title(f"{file_path.name} - ODE Visualizer")
    plt.show()

def run_cli(csv_dir: Path, output_dir: Path):    
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt
    if not csv_dir.is_dir(): print(f"Error: Input directory {csv_dir} does not exist or is not a folder."); sys.exit(1)
    csv_files = list(csv_dir.glob("*.csv"))
    if not csv_files: print(f"No matching CSV files found in {csv_dir}"); return
    output_dir.mkdir(parents=True, exist_ok=True)
    print(f"Found {len(csv_files)} CSV files. Starting batch rendering...")
    for csv_path in csv_files:
        output_path = output_dir / f"{csv_path.stem}.png"
        fig = init_fig(csv_path)
        fig.savefig(output_path, dpi=300)
        plt.close(fig)
        print(f"  [+] Saved: {output_path.name}")
    print(f"Success: All plots saved to {output_dir}")

    
def main():
    if len(sys.argv) <= 1:
        run_gui()
    elif len(sys.argv) == 3:
        run_cli(Path(sys.argv[1]), Path(sys.argv[2]))
    else:
        print("Usage:")
        print("  GUI mode: python script.py")
        print("  CLI mode: python script.py <input_csv_folder> <output_image_folder>")
        sys.exit(1)

if __name__ == "__main__":
    main()
