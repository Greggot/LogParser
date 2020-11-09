from tkinter import *
from tkinter import ttk
from tkinter.filedialog import askopenfilename
import os


root = Tk()
root.title("Parser")
root.resizable(True, True)

prods = ['TRW', 'Bosch', 'Delphi']
servs = ['Upload', 'Download']

path = ''

def runMain():
    os.system('main')

SettingsFrame = ttk.LabelFrame(root, text='Settings')
SettingsFrame.grid(column=0, row=0, sticky='nw')

ttk.Label(SettingsFrame, text='Producer:').grid(column=0,row=0, sticky='w', padx=3)
ttk.Label(SettingsFrame, text='Service:').grid(column=0,row=1, sticky='w', padx=3, pady=3)

ttk.Combobox(SettingsFrame, values=prods, state="readonly").grid(column=1, row=0, sticky='w')
ttk.Combobox(SettingsFrame, values=servs, state="readonly").grid(column=1, row=1, sticky='w', pady=3)


ttk.Label(SettingsFrame, text='*.txt files only').grid(column=0,row=4, sticky='w', padx=3)
PathLabel = Label(SettingsFrame, text=path)
PathLabel.grid(column=1,row=3, sticky='w', pady=5)

def setInputLog():
    path = askopenfilename()
    PathLabel.config(text = path)

SelectFileButton = Button(SettingsFrame, text="Select a file", command=setInputLog, pady=5)
SelectFileButton.grid(column=0,row=3, sticky='w')

def Parse():
    outputPath = 'C'+PathLabel["text"][1:len(path)-3]+'bin'
    print('ParserVolvo '+PathLabel["text"]+' '+outputPath)
    os.system('ParserVolvo'+' '+PathLabel["text"]+' '+outputPath)

message_button = Button(root, text="Filter data", command=Parse)
message_button.grid(column=0,row=2, sticky='se', padx=3, pady=3)

root.mainloop()
