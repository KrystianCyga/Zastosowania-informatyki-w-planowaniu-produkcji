import tkinter as tk
from tkinter import ttk
import requests
import json
import random

def optimize():
    global result_label
    try:
        liczba_zadan = int(entry_zadan.get())
        liczba_maszyn = int(entry_maszyn.get())
        temperatura_poczatkowa = float(entry_temp.get())
        wspolczynnik_chlodzenia = float(entry_chlodzenie.get())
        liczba_iteracji = int(entry_iteracje.get())

        czasy_przetwarzania = []
        for i in range(liczba_zadan):
            row = []
            for j in range(liczba_maszyn):
                row.append(int(entries[i][j].get()))
            czasy_przetwarzania.append(row)

        data = {
            "liczba_zadan": liczba_zadan,
            "liczba_maszyn": liczba_maszyn,
            "czasy_przetwarzania": czasy_przetwarzania,
            "temperatura_poczatkowa": temperatura_poczatkowa,
            "wspolczynnik_chlodzenia": wspolczynnik_chlodzenia,
            "liczba_iteracji": liczba_iteracji
        }

        response = requests.post("http://localhost:8080/optimize", json=data)
        result = response.json()

        result_label.config(text=f"Najlepsze rozwiązanie: {result['najlepsze_rozwiazanie']}\nNajlepszy czas: {result['najlepszy_czas']}")
    except Exception as e:
        result_label.config(text=f"Błąd: {str(e)}")

def generate_table():
    global entries, result_label
    entries = []
    try:
        liczba_zadan = int(entry_zadan.get())
        liczba_maszyn = int(entry_maszyn.get())
        
        # Usunięcie poprzednich widgetów
        for widget in root.winfo_children():
            if isinstance(widget, ttk.Entry) and widget not in [entry_zadan, entry_maszyn, entry_temp, entry_chlodzenie, entry_iteracje]:
                widget.destroy()
            if isinstance(widget, ttk.Label) and widget.grid_info()['row'] >= 6:
                widget.destroy()
        
        # Dodanie nagłówków kolumn (maszyny)
        for j in range(liczba_maszyn):
            ttk.Label(root, text=f"M{j+1}").grid(row=5, column=j+1)
        
        for i in range(liczba_zadan):
            row = []
            # Dodanie etykiety wiersza (zadania)
            ttk.Label(root, text=f"Z{i+1}").grid(row=i+6, column=0)
            for j in range(liczba_maszyn):
                entry = ttk.Entry(root, width=5)
                entry.grid(row=i+6, column=j+1)
                entry.insert(0, str(random.randint(1, 10)))  # Losowa wartość między 1 a 10
                row.append(entry)
            entries.append(row)
        
        ttk.Button(root, text="Optymalizuj", command=optimize).grid(row=liczba_zadan+6, column=0, columnspan=liczba_maszyn+1)
        
        # Odtworzenie result_label
        result_label = ttk.Label(root, text="")
        result_label.grid(row=liczba_zadan+7, column=0, columnspan=liczba_maszyn+1)
        
    except ValueError:
        print("Proszę wprowadzić poprawne liczby zadań i maszyn.")

root = tk.Tk()
root.title("Symulowane wyżarzanie - interfejs")

ttk.Label(root, text="Liczba zadań:").grid(row=0, column=0)
entry_zadan = ttk.Entry(root)
entry_zadan.grid(row=0, column=1)

ttk.Label(root, text="Liczba maszyn:").grid(row=1, column=0)
entry_maszyn = ttk.Entry(root)
entry_maszyn.grid(row=1, column=1)

ttk.Label(root, text="Temperatura początkowa:").grid(row=2, column=0)
entry_temp = ttk.Entry(root)
entry_temp.grid(row=2, column=1)

ttk.Label(root, text="Współczynnik chłodzenia:").grid(row=3, column=0)
entry_chlodzenie = ttk.Entry(root)
entry_chlodzenie.grid(row=3, column=1)

ttk.Label(root, text="Liczba iteracji:").grid(row=4, column=0)
entry_iteracje = ttk.Entry(root)
entry_iteracje.grid(row=4, column=1)

entry_zadan.insert(0, "5")  # przykładowa liczba zadań
entry_maszyn.insert(0, "3")  # przykładowa liczba maszyn
entry_temp.insert(0, "1000.0")  # przykładowa temperatura początkowa
entry_chlodzenie.insert(0, "0.98")  # przykładowy współczynnik chłodzenia
entry_iteracje.insert(0, "100")  # przykładowa liczba iteracji

ttk.Button(root, text="Generuj tabelę", command=generate_table).grid(row=5, column=0, columnspan=2)

result_label = ttk.Label(root, text="")
result_label.grid(row=100, column=0, columnspan=2)

root.mainloop()