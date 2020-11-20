#**************************************************************************************************
# Includes
#**************************************************************************************************

from tkinter import *
from tkinter import ttk
from tkinter import messagebox
from tkinter.filedialog import askopenfilename
from tkinter.filedialog import asksaveasfilename
import subprocess
import os

#**************************************************************************************************
# Definitions
#**************************************************************************************************

path = ''

#**************************************************************************************************
# Window Structures (Widgets)
#**************************************************************************************************

root = Tk()
root.title("Parser")                # Название окна
root.geometry('640x300+100+100')    # Размеры окна и расстояние от краёв экрана
root.resizable(False, False)        # Запрет на изменение размеров окна

prods = ['Delphi', 'TRW']  # Список производителей
servs = {'Delphi' : 'ReadMemoryByAddress', 'TRW' : 'TransferData'}

SettingsFrame = ttk.LabelFrame(root, text = 'Settings', width = 300)            # Рамка, внутри которой находится настройки
SettingsFrame.pack(fill = X)

ChooseFrame = Frame(SettingsFrame, borderwidth = 0, highlightthickness = 0)     # Рамки выбора производителя и режима
ChooseFrame.pack(side = TOP, pady = 5)

UpperChoose = Frame(ChooseFrame, borderwidth = 0, highlightthickness = 0)       # Рамка выбора производителя
UpperChoose.pack(side = TOP)

DownChoose = Frame(ChooseFrame, borderwidth = 0, highlightthickness = 0)        # Рамка выбора режима. Все раки нужны, чтобы нормально отобразить элементы в центре
DownChoose.pack(side = TOP, pady = 5)

ttk.Label(UpperChoose, text = 'Producer:').pack(side = LEFT)        # Надпись "Производитель"
ttk.Label(DownChoose, text='Service:    ').pack(side = LEFT, pady = 5)  # Надпись "Режим"

ProdBox = ttk.Combobox(UpperChoose, values = prods, state = "readonly")             # Выбор производителя
ProdBox.pack(side = RIGHT, padx = 10)
ServBox = ttk.Combobox(DownChoose, values = None, state = "readonly")    # Выбор режима
ServBox.pack(side = RIGHT, pady = 5, padx = 10)
ServBox.set("-Select servise-")

def ServsChange(event):
    temp = servs[ProdBox.get()]
    ServBox.set("-Select servise-")
    ServBox['values'] = temp
    
ProdBox.bind("<<ComboboxSelected>>", ServsChange)

SelectFrame = Frame(SettingsFrame, borderwidth = 0, highlightthickness = 0)     #Рамка выбора файла
SelectFrame.pack(side = BOTTOM, pady = 5)

path = StringVar()

PathEntry = Entry(SelectFrame, textvariable = path, width=100)         # Тектовое поле, хранящее путь к файлу
PathEntry.pack(side = BOTTOM, pady = 5, padx = 5)

#**************************************************************************************************
# Procedure setInputLog()
#**************************************************************************************************

# Считывание пути к входному файлу  #

def setInputLog():
    Path = askopenfilename(
        filetypes=((".txt files", "*.txt"),))   # Открывать только файлы формата .txt
    PathEntry.delete(0, END)                    # Стереть старый путь
    PathEntry.insert(0, Path)                   # Вставить новый

CheckFrame = Frame(SelectFrame, borderwidth = 0, highlightthickness = 0)
CheckFrame.pack(side = BOTTOM, pady = 5)

SelectFileButton = Button(SelectFrame, text = "Select a file", command = setInputLog)   # Кнопка, считывающая путь к файлу
SelectFileButton.pack(side = TOP, pady = 5, padx = 5)

showTable = IntVar()
Checkbutton(CheckFrame, text = "Show Table", variable = showTable).pack(side = LEFT, pady = 5, padx = 5)

def ParseTable():
    outputPath = asksaveasfilename(filetypes=[("Binary File", "*.bin")])
    if(outputPath != ''):
        proc = subprocess.Popen('Parser' + ProdBox.get() + ServBox.get() + ' ' + path.get() + ' ' + outputPath + '.bin',  creationflags = subprocess.SW_HIDE, shell = True)  # Запуск программы извне
        print('Parser' + ProdBox.get() + ServBox.get() + ' ' + path.get() + ' ' + outputPath + '.bin')
        proc.wait()     # Ожидание конца выполнения
        if os.path.isfile(outputPath + '.bin'):          # Если после выполнения нашёлся бинарный файл с нужны именем, то програма выполнилась правильно 
            messagebox.showinfo('Success', 'File has been generated at ' + outputPath + '.bin')  # Сообщение с информацией об успехе
        else:
            messagebox.showerror('Error', 'File hasn\'t been generated!')   # Сообщение-ошибка
        


#**************************************************************************************************
# Procedure Parse()
#**************************************************************************************************

# Запускает из консоли программу ParseVolvo #

def Parse():
    if(showTable.get() == 1):
        Table = Tk()
        Table.title("Table")                # Название окна
        Table.geometry('860x300+200+200')    # Размеры окна и расстояние от краёв экрана
        Table.resizable(False, False)        # Запрет на изменение размеров окна

        DataFrame = Frame(Table, borderwidth = 0, highlightthickness = 0).pack(side = TOP)

        cmbBoxfile = open('temp/ID.txt', 'r')
        cmbBoxValues = cmbBoxfile.read()

        ServerBox = ttk.Combobox(Table, values = cmbBoxValues, state = "readonly")             # Выбор ID
        ServerBox.pack(side = TOP, padx = 10)
        ServerBox.set("-Select server-")
        ClientBox = ttk.Combobox(Table, values = cmbBoxValues, state = "readonly")             # Выбор ID
        ClientBox.pack(side = TOP, padx = 10)
        ClientBox.set("-Client server-")
        
        ButtonFrame = Frame(Table, borderwidth = 0, highlightthickness = 0)
        ButtonFrame.pack(side = BOTTOM, pady = 5)

        
        button = Button (Table, text = "   Parse   ", command = ParseTable)
        button.pack(side = BOTTOM, padx = 5)

        log = open('temp/LOG.txt', 'r')
        logdata = log.read()
        
        textbox = Text(Table, width = 85)
        textbox.insert(INSERT, logdata)
        textbox.config(state=DISABLED)
        textbox.pack(side = LEFT)

        scrollbar = Scrollbar(Table)
        scrollbar.pack(side=LEFT, fill=Y)

        IDtextbox = Text(Table, width = 8)
        IDtextbox.insert(INSERT, cmbBoxValues)
        IDtextbox.config(state=DISABLED)
        IDtextbox.pack(side = LEFT, padx = 7)
        
        #for i in range(100):
        #    textbox.insert(END, f"This is an example line {i}\n")
        # attach textbox to scrollbar
        textbox.config(yscrollcommand=scrollbar.set)
        scrollbar.config(command=textbox.yview)
        
        Table.mainloop()
    else:
        print(0)
    
message_button = Button(root, text = "Filter", command = Parse)    # Кнопка, запускающая Parse()
message_button.pack()

root.mainloop()         # Функция, обновляющая виджеты на экране
