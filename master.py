import tkinter as tk
from tkinter import ttk
import serial
from utils import send_to_arduino, read_from_arduino

root = tk.Tk()
root.title('Gate Control System')
root.geometry('300x330')
root.resizable(False, False)

MASTER_PORT = 'COM5'
master_ser = serial.Serial(MASTER_PORT, 115200)

key_var = tk.StringVar()
gate_number = tk.StringVar()

def copy_label_value():
    label_text = key_var.get()

    root.clipboard_clear()
    root.clipboard_append(label_text)
    root.update()
    
    key_var.set("Copied")
    root.after(1000, reset_func)

def reset_func():
    key_var.set("")
    reg_btn.configure(state="active")
    

def generate_token():
    if(len(gate_number.get()) != 0):
        print(gate_number.get()[-1])
        send_to_arduino(gate_number.get()[-1], master_ser)

        while (master_ser.inWaiting() == 0):
            pass

        token = read_from_arduino(master_ser)
        print(token)
        key_var.set(token)
        reg_btn.configure(state='disabled')

heading = ttk.Label(root, text='MASTER', font='arial 20 bold')
heading.pack(pady=10)

ttk.Label(root).pack()

nic_label = ttk.Label(root, text='NIC')
nic_entry = ttk.Entry(root)
nic_label.pack()
nic_entry.pack()

ttk.Label(root).pack()

gate_label = ttk.Label(root, text='Select Slave')
combobox = ttk.Combobox(root, values=["Slave 1", "Slave 2"], textvariable=gate_number, state="readonly")
gate_label.pack()
combobox.pack()

ttk.Label(root).pack(pady='20')

reg_btn = ttk.Button(root,text="Generate Token", command=lambda:generate_token())
reg_btn.pack(ipadx=20, ipady=10)

key_var.set("")

key_label = ttk.Label(root, textvariable=key_var, cursor="hand2")
key_label.pack(side='bottom', pady='15')

key_label.bind("<Button-1>", lambda event: copy_label_value())

root.mainloop()