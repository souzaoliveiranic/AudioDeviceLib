# AudioDeviceLib

Para utilizar essa biblioteca você deve estender a classe abstrata AudioDeviceLib. 
Alguns métodos são obrigatórios de serem chamadas durante a inicialização e estão descritos no cabeçalho principal da biblioteca.
Os sounds cards devem ser passados para a biblioteca e as teclas de PTT devem ser configuradas conforme descrição do método. 
A interface de udp serve tanto para receber quanto enviar amostras.

Esta biblioteca depende de outras bibliotecas, sendo elas: util (presente neste repositório), m, asound, pthread, rt, liquid, gpiod.
