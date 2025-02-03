# Preparação no VSCode

- **1°:** clone o repositório para o seu computador.

    - Ao abrir o projeto com o **VSCode**, use a extensão do **Raspberry Pi Pico** e importe o **CMake** no diretório do projeto.

    - Caso não seja gerada a pasta, crie uma pasta com nome `build` e execute o seguinte comando dentro da pasta criada:
        
        ``cmake ..``

        O comando acima irá criar os arquivos de compilação.

- **2°:** execute a compilação do firmware usando a extensão do ***Raspberry Pi Pico*** do ***VSCode***.

A partir daqui, seu firmware já está compilado e pronto para uso.

## Execução na *BitDogLab*

- **1°:** coloque o seu ***Raspberry*** em modo de ***bootsel***, ou seja, segure o botão de ***bootsel*** (localizado no RP2040) enquanto aperta o botão de ***reset*** (localizado abaixo do *joystick*).

- **2°:** já conectado na sua máquina via usb, copie o arquivo `.uf2` que foi gerado ao compilar e cole no armazenamento externo que aparecerá ao conectar a placa, com se fosse um Pen-Drive com nome de RPI-RP2.

    - Após isso, o seu código já vai está rodando na sua plaquinha ***BitDogLab***.

- **3°:** Está pronto, dê os comandos para executar as funcionalidades clicando nas teclas do seu teclado.


## Comandos disponívels:

`Push Button A`: Incrementa o número exibido e muda a cor dos LEDs para verde.

`Push Button B`: Decrementa o número exibido e muda a cor dos LEDs para vermelho.

## Vídeo de demonstração 

Clique em ***[link do video](https://youtu.be/PBIahsWWTp4?si=qs6VQHHsYKKJkBPD)*** para visualizar o vídeo ensaio do projeto.
