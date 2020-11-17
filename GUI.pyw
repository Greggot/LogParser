#**************************************************************************************************
# Includes
#**************************************************************************************************

from tkinter import *
from tkinter import ttk
from tkinter import messagebox
from tkinter.filedialog import askopenfilename
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
root.geometry('640x300+200+100')    # Размеры окна и расстояние от краёв экрана
root.resizable(False, False)        # Запрет на изменение размеров окна

prods = ['TRW', 'Bosch', 'DAF']  # Список производителей
servs = ['Upload', 'Download']      # Список режимов

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
ServBox = ttk.Combobox(DownChoose, values = servs, state = "readonly")    # Выбор режима
ServBox.pack(side = RIGHT, pady = 5, padx = 10)

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

SelectFileButton = Button(SelectFrame, text = "Select a file", command = setInputLog)   # Кнопка, считывающая путь к файлу
SelectFileButton.pack(side = TOP, pady = 5, padx = 5)

#**************************************************************************************************
# Procedure Parse()
#**************************************************************************************************

# Запускает из консоли программу ParseVolvo #

def Parse():
    outputPath = path.get()[0:len(path.get())-3] + 'bin'     #Путь к бинарнику будет тем же, что и уисходного файла. Отличаться будет только формат
    proc = subprocess.Popen('Parser' + ProdBox.get() + ' ' + path.get() + ' ' + outputPath,  creationflags = subprocess.SW_HIDE, shell = True)  # Запуск программы извне
    print ('Parser' + ProdBox.get() + ' ' + path.get() + ' ' + outputPath)
    proc.wait()     # Ожидание конца выполнения
    if os.path.isfile(outputPath):          # Если после выполнения нашёлся бинарный файл с нужны именем, то програма выполнилась правильно 
        messagebox.showinfo('Success', 'File has been generated at ' + outputPath)  # Сообщение с информацией об успехе
    else:
        messagebox.showerror('Error', 'File hasn\'t been generated!')   # Сообщение-ошибка

message_button = Button(root, text = "Filter data", command = Parse)    # Кнопка, запускающая Parse()
message_button.pack()

root.mainloop()         # Функция, обновляющая виджеты на экране