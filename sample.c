from PySide2.QtWidgets import *

import pymxs
from pymxs import runtime as rt

MAX_WINDOW = QWidget.find(rt.windows.getMAXHWND())


class AutoDialogClosure:
    
    _instances:'AutoDialogClosure' = []
    
    DEBUG_MODE = False # No click button and print title and button text
    
    TARGET_WIDGETS = (
        QMessageBox,
        QDialog,
        QWidget,
    )

    TARGET_DIALOGS = {
        "警告: ファイルは保存されません": "はい(&Y)",
        "ファイルの存在": "はい",
        "分離ツール": "OK"
    }
        
    def __init__(self):
        
        for ins in AutoDialogClosure._instances:
            del(ins)
            
        AutoDialogClosure._instances.append(self)
        
        
        self.require_japanese_max()
        self._original_monitor_enabled = rt.DialogMonitorOPS.Enabled
        
        self.stop()
        
        rt.__RR_ADC_CALLBACK_PY__ = self._dialog_closure
        rt.execute('fn __RR_ADC_CALLBACK__ = (__RR_ADC_CALLBACK_PY__())')
        
        
    def __del__(self):
        print("swl")
        self.stop()
        
        
    def start(self):
        rt.DialogMonitorOPS.UnRegisterNotification(ID=rt.name("RR_AutoDialogClosure"))
        rt.DialogMonitorOPS.RegisterNotification(rt.__RR_ADC_CALLBACK__, ID=rt.name("RR_AutoDialogClosure"))
        rt.DialogMonitorOPS.Enabled = True
        
    def stop(self):
        print("stop")
        rt.DialogMonitorOPS.UnRegisterNotification(ID=rt.name("RR_AutoDialogClosure"))
        rt.DialogMonitorOPS.Enabled = self._original_monitor_enabled

        
    def _dialog_closure(self, *args):
        
        hwnd = rt.DialogMonitorOPS.GetWindowHandle()

        if hwnd == 0:
            return True
            
        qt_window = MAX_WINDOW.find(hwnd)
        
        if qt_window is None:
            
            title = rt.UIAccessor.GetWindowText(hwnd)
            
            if not title.strip():
                return True
            
            if self.DEBUG_MODE:
                print(f"WindowTitle: {title}")
            
            if title not in self.TARGET_DIALOGS and not self.DEBUG_MODE:
                return True
            
            for child_hwnd in rt.UIAccessor.GetChildWindows(hwnd):
                
                if rt.UIAccessor.GetWindowClassName(child_hwnd) != "Button":
                    continue
                    
                button_text = rt.UIAccessor.GetWindowText(child_hwnd)
                
                if self.DEBUG_MODE:
                    print(f"    ButtonText: {button_text}")
                    
                if not self.DEBUG_MODE and  self.TARGET_DIALOGS[title] == button_text:
                    rt.UIAccessor.PressButton(child_hwnd)
                    break
                    
            return True
        
        elif isinstance(qt_window, self.TARGET_WIDGETS):
                    
            title = qt_window.windowTitle()
            
            if not title.strip():
                return True
            
            if self.DEBUG_MODE:
                print(f"WindowTitle: {title}")
            
            if not self.DEBUG_MODE and title not in self.TARGET_DIALOGS:
                return True
            
            
            for child in qt_window.findChildren(QPushButton):
                
                button_text = child.text()
                
                if self.DEBUG_MODE:
                    print(f"    ButtonText: {button_text}")
                    
                if not self.DEBUG_MODE and button_text == self.TARGET_DIALOGS[title]:
                    child.click()
                    break
        
        elif self.DEBUG_MODE:
            print("HWND:", hwnd, "Qt:", qt_window)
                
                
        return True
    
    
    def require_japanese_max(self) -> None:
        result = rt.sysInfo.getLanguage()
        if "ja-JP" not in result:
            raise RuntimeError("Only Japanese 3dsMax")

rt.AAA = AutoDialogClosure()
rt.AAA.start()
#~ a.stop()

