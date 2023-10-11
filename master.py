import tkinter as tk
from tkinter import ttk
import serial
from utils import send_to_arduino, read_from_arduino
from logger import logging

root = tk.Tk()
root.title('Gate Controller System')
root.geometry('300x330')
root.resizable(False, False)

MASTER_PORT = 'COM5'
master_ser = serial.Serial(MASTER_PORT, 115200)
logging.info("Port Connected")

key_var = tk.StringVar()
gate_number = tk.StringVar()
nic_number = tk.StringVar()

def copy_label_value():
    label_text = key_var.get()
    logging.info(f"Copied token: {label_text}")

    root.clipboard_clear()
    root.clipboard_append(label_text)
    root.update()
    
    key_var.set("Copied")
    root.after(1000, reset_func)

def reset_func():
    key_var.set("")
    reg_btn.configure(state="active")
    

def generate_token():
    if(len(nic_number.get()) >= 9 and nic_number.get()[:9].isdigit()):
        if(len(gate_number.get()) != 0):
            msg = nic_number.get()[:9] + gate_number.get()[-1]

            logging.info(f"First 9 characters from NIC: {nic_number.get()[:9]}")
            logging.info(f"Slave ID: {gate_number.get()[-1]}")

            send_to_arduino(msg, master_ser)

            logging.info(f"Send to Microcontroller: {msg}")

            while (master_ser.inWaiting() == 0):
                pass

            token = read_from_arduino(master_ser)
            logging.info(f"Read token from Microcontroller: {token}")

            key_var.set(token)
            reg_btn.configure(state='disabled')
        else:
            key_var.set("Slave not selected")
            logging.error("Slave not selected")
            root.after(1000, reset_func)
    else:
        key_var.set("Invalid NIC number")
        logging.error("Invalid NIC number")
        root.after(1000, reset_func)

heading = ttk.Label(root, text='MASTER', font='arial 20 bold')
heading.pack(pady=10)

ttk.Label(root).pack()

nic_label = ttk.Label(root, text='NIC')
nic_entry = ttk.Entry(root, textvariable=nic_number)
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