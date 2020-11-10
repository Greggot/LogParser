from tkinter import *
from tkinter import ttk
from tkinter import messagebox
from tkinter.filedialog import askopenfilename
import subprocess
import os

root = Tk()
root.title("Parser")
root.geometry('640x300+200+100')
root.resizable(False, False)

prods = ['TRW', 'Bosch', 'Delphi']
servs = ['Upload', 'Download']

path = ''

SettingsFrame = ttk.LabelFrame(root, text = 'Settings', width = 300)
SettingsFrame.pack(fill = X)

ChooseFrame = Frame(SettingsFrame, borderwidth = 0, highlightthickness = 0)
ChooseFrame.pack(side = TOP, pady = 5)

UpperChoose = Frame(ChooseFrame, borderwidth = 0, highlightthickness = 0)
UpperChoose.pack(side = TOP)

DownChoose = Frame(ChooseFrame, borderwidth = 0, highlightthickness = 0)
DownChoose.pack(side = TOP, pady = 5)

ttk.Label(UpperChoose, text = 'Producer:').pack(side = LEFT)
ttk.Label(DownChoose, text='Service:').pack(side = LEFT, pady = 5)

ttk.Combobox(UpperChoose, values = prods, state = "readonly").pack(side = RIGHT, padx = 10)
ttk.Combobox(DownChoose, values = servs, state = "readonly").pack(side = RIGHT, pady = 5, padx = 17)

SelectFrame = Frame(SettingsFrame, borderwidth = 0, highlightthickness = 0)
SelectFrame.pack(side = BOTTOM, pady = 5)

PathLabel = Label(SelectFrame, text = path)
PathLabel.pack(side = RIGHT, pady = 5, padx = 5)

def setInputLog():
    path = askopenfilename(
        filetypes=((".txt files", "*.txt"),))
    PathLabel.config(text = path)

SelectFileButton = Button(SelectFrame, text = "Select a file", command = setInputLog)
SelectFileButton.pack(side = TOP, pady = 5, padx = 5)

def Parse():
    outputPath = 'C' + PathLabel["text"][1:len(path)-3] + 'bin'
    proc = subprocess.Popen('ParserVolvo' + ' ' + PathLabel["text"] + ' ' + outputPath,  creationflags = subprocess.SW_HIDE, shell = True)
    proc.wait()
    if os.path.isfile(outputPath):
        messagebox.showinfo('Success', 'File has been generated at ' + outputPath)
    else:
        messagebox.showerror('Error', 'File hasn\'t been generated!')

message_button = Button(root, text = "Filter data", command = Parse)
message_button.pack()

root.mainloop()
