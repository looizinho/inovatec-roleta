import serial
import pyautogui
import tkinter as tk
from tkinter import scrolledtext

PORTA_SERIAL = "COM3"
VELOCIDADE_SERIAL = 115200

ATALHOS = {
    "sensor1": "1",
    "sensor2": "2",
    "sensor3": "3",
    "sensor4": "4",
    "sensor5": "5",
    "sensor6": "6",
    "sensor7": "7",
}


class App:
    def __init__(self, master):
        self.master = master
        self.master.title("Controle Museu do Amanhã")
        self.ser = None
        self.serial_conectado = False

        # Área de log com barra de rolagem
        self.log = scrolledtext.ScrolledText(
            master, width=50, height=12, state="normal"
        )
        self.log.pack(padx=10, pady=10)

        self.log.tag_configure("serial", foreground="#E72AEB")   # azul para serial
        self.log.tag_configure("atalho", foreground="#E9CF0D")   # verde para atalho

        # Botões
        self.btn_conectar = tk.Button(
            master, text="Conectar", command=self.conectar_serial
        )
        self.btn_conectar.pack(side="left", padx=10, pady=10)

        self.btn_desconectar = tk.Button(
            master,
            text="Desconectar",
            command=self.desconectar_serial,
            state="disabled",
        )
        self.btn_desconectar.pack(side="left", padx=10, pady=10)

        self.btn_auto_teste = tk.Button(
            master,
            text="Iniciar Auto Teste",
            command=self.auto_teste,
            state="normal",
        )
        self.btn_auto_teste.pack(side="left", padx=10, pady=10)

        self.master.protocol("WM_DELETE_WINDOW", self.fechar_janela)

    def conectar_serial(self):
        try:
            self.ser = serial.Serial(PORTA_SERIAL, VELOCIDADE_SERIAL, timeout=1)
            self.serial_conectado = True
            self.log.config(state="normal")
            self.log.delete('1.0', tk.END)
            self.log.config(state="normal")
            self.log_insert(f"Conectado à porta {PORTA_SERIAL}")
            self.btn_conectar.config(state="disabled")
            self.btn_desconectar.config(state="normal")
            self.master.after(100, self.leitura_serial)
        except serial.SerialException as e:
            self.log_insert(f"Erro ao conectar: {e}")

    def desconectar_serial(self):
        if self.ser and self.ser.is_open:
            self.ser.close()
        self.serial_conectado = False
        self.btn_conectar.config(state="normal")
        self.btn_desconectar.config(state="disabled")
        self.log_insert("Desconectado da porta serial.")

    def leitura_serial(self):
        if self.serial_conectado and self.ser and self.ser.is_open:
            try:
                while self.ser.in_waiting:
                    linha = self.ser.readline().decode("utf-8").strip()
                    self.log_insert(f"Recebido: {linha}", "serial")
                    atalho = ATALHOS.get(linha.lower())
                    if atalho:
                        pyautogui.press(atalho)
                        self.log_insert(f"Atalho enviado: {atalho}", "atalho")
            except Exception as e:
                self.log_insert(f"Erro na leitura: {e}")
            self.master.after(50, self.leitura_serial)

    def log_insert(self, texto, tag=None):
        self.log.config(state="normal")
        if tag:
            self.log.insert(tk.END, texto + "\n", tag)
        else:
            self.log.insert(tk.END, texto + "\n")
        self.log.see(tk.END)
        self.log.config(state="normal")

    def fechar_janela(self):
        if self.ser and self.ser.is_open:
            self.ser.close()
        self.master.destroy()

    def auto_teste(self):
        if getattr(self, "auto_teste_rodando", False):
            # Parar o auto teste
            self.auto_teste_rodando = False
            self.btn_auto_teste.config(text="Iniciar Auto Teste")
            self.log_insert("Auto Teste parado.")
            return

        # Se estiver conectado à serial, desconecta
        if self.ser and self.ser.is_open:
            self.ser.close()
            self.serial_conectado = False
            self.btn_conectar.config(state="normal")
            self.btn_desconectar.config(state="disabled")
            self.log_insert("Serial desconectada para Auto Teste.")

        self.log.config(state="normal")
        self.log.delete('1.0', tk.END)
        self.log.config(state="normal")

        # Inicia o auto teste
        self.log_insert("Iniciando Auto Teste...")
        self.auto_teste_keys = list(ATALHOS.values())
        self.auto_teste_index = 0
        self.auto_teste_rodando = True
        self.btn_auto_teste.config(text="Parar Auto Teste")
        self.auto_teste_loop()

    def auto_teste_loop(self):
        if not getattr(self, "auto_teste_rodando", False):
            return
        if self.auto_teste_index >= len(self.auto_teste_keys):
            self.auto_teste_index = 0
        atalho = self.auto_teste_keys[self.auto_teste_index]
        pyautogui.press(atalho)
        self.log_insert(f"Auto Teste: atalho enviado {atalho}", "atalho")
        self.auto_teste_index += 1
        # Chama de novo em 1 segundo
        self.master.after(1000, self.auto_teste_loop)


if __name__ == "__main__":
    root = tk.Tk()
    app = App(root)
    root.mainloop()
