#**************************************************************************************************
# Includes
#**************************************************************************************************

from tkinter import *
from tkinter import ttk
from tkinter.filedialog import askopenfilename
import os

#**************************************************************************************************
# Definitions
#**************************************************************************************************

path = ''

#**************************************************************************************************
# Window Structures (Widgets)
#**************************************************************************************************

root = Tk()                 #Создаём объект окна
root.title("Parser")        #Название
root.resizable(False, False)  #Масштабируется ли по х, по у

prods = ['TRW', 'Bosch', 'Delphi']  #Список производителей
servs = ['Upload', 'Download']      #Список режимов

SettingsFrame = ttk.LabelFrame(root, text = 'Settings')   #Фрейм для объектов настройки, на нём помещаются виджеты
SettingsFrame.grid(column = 0, row = 0, sticky = 'nw')

ttk.Label(SettingsFrame, text='Producer:').grid(column = 0, row = 0, sticky = 'w', padx = 3)         #Надпись "Производители"
ttk.Label(SettingsFrame, text='Service:').grid(column = 0, row = 1, sticky = 'w', padx = 3, pady = 3)  #Надпись "Сервисы"

ttk.Combobox(SettingsFrame, values = prods, state = "readonly").grid(column = 1, row = 0, sticky = 'w')           #Выбор производителей
ttk.Combobox(SettingsFrame, values = servs, state = "readonly").grid(column = 1, row = 1, sticky = 'w', pady = 3)   #Выбор режимов


ttk.Label(SettingsFrame, text = '*.txt files only').grid(column = 0, row = 4, sticky = 'w', padx = 3) 
PathLabel = Label(SettingsFrame, text=path)         #Надпись, отображающая путь файла
PathLabel.grid(column = 1, row = 3, sticky='w', pady = 5)

#**************************************************************************************************
# Procedure setInputLog()
#**************************************************************************************************

# Считывание пути к входному файлу  #

def setInputLog():
    path = askopenfilename()
    PathLabel.config(text = path)   #Изменить текст надписи, отображающей путь файла

SelectFileButton = Button(SettingsFrame, text = "Select a file", command = setInputLog, pady = 5)     #Кнопка для выбора файла, запускает setInputLog()
SelectFileButton.grid(column = 0, row = 3, sticky = 'w')


#**************************************************************************************************
# Procedure Parse()
#**************************************************************************************************

# Запускает из консоли программу ParseVolvo #

def Parse():
    outputPath = 'C' + PathLabel["text"][1:len(path) - 3] + 'bin'   #Выходной файл будет находиться там же, что и входной, только иметь другое разрешение
    os.system('ParserVolvo' + ' ' + PathLabel["text"] + ' ' + outputPath)   #запустить через терминал парсер

message_button = Button(root, text = "Filter data", command = Parse)    #Кнопка, выполняющая Parse()
message_button.grid(column = 0, row = 2, sticky = 'se', padx = 3, pady = 3)

root.mainloop()         #Функция, обновляющая виджеты на экране
