# JQBSTM32_Framework

![Status projektu](https://img.shields.io/badge/status-w%20rozwoju-yellow)
![Licencja](https://img.shields.io/badge/licencja-GPL-blue)

## Opis projektu

JQBSTM32_Framework to framework ułatwiający rozwój aplikacji na mikrokontrolery STM32. Umożliwia szybkie i efektywne tworzenie projektów, zarządzanie zasobami i optymalizację kodu. Framework zawiera zestaw bibliotek i narzędzi wspierających różne aspekty programowania mikrokontrolerów STM32.

**Uwaga:** Projekt jest nadal w fazie rozwoju.

## Instalacja

Aby zainstalować JQBSTM32_Framework, wykonaj następujące kroki:

1. **Dodanie jako submoduł**

    ```bash
    git submodule add https://github.com/JAQUBA/JQBSTM32_Framework.git
    cd JQBSTM32_Framework
    ```

2. **Instalacja zależności**

    - Upewnij się, że masz zainstalowane wymagane narzędzia i biblioteki. Szczegółowe instrukcje znajdują się w sekcji [Wymagania](#wymagania).

## Użycie

Aby użyć JQBSTM32_Framework w swoim projekcie, postępuj zgodnie z poniższymi krokami:

1. **Konfiguracja środowiska**
    - Zainstaluj Visual Studio Code oraz rozszerzenie PlatformIO.
    - Skonfiguruj STM32CubeMX do generowania kodu startowego dla Twojego projektu.

2. **Tworzenie nowego projektu**
    - Skorzystaj z dostarczonych skryptów i szablonów projektów, aby szybko rozpocząć pracę.

3. **Struktura katalogów**
    - Zasoby sprzętowe znajdują się w katalogu `Hardware`.
    - Biblioteki do obsługi peryferiów znajdują się w katalogu `Utils`.

4. **STM32CubeMX**: Projekt został skonfigurowany za pomocą STM32CubeMX. Można go dalej dostosować przy użyciu tego narzędzia.

5. **Visual Studio Code i PlatformIO**: Do kompilacji i wgrywania oprogramowania na urządzenie wymagane jest środowisko Visual Studio Code z wtyczką PlatformIO.

## Wymagania

- **System operacyjny**: Windows, macOS, Linux
- **Narzędzia**:
    - STM32CubeMX
    - Visual Studio Code z rozszerzeniem PlatformIO
    - Kompilator ARM (np. arm-none-eabi-gcc)
    - Narzędzia do debugowania (np. OpenOCD, ST-Link)
- **Biblioteki**:
    - CMSIS
    - STM32 HAL

## Przykładowy projekt

### Krok 1: Konfiguracja STM32CubeMX

1. Uruchom STM32CubeMX.
2. Wybierz mikrokontroler: `STM32F103C8Tx`.
3. Skonfiguruj peryferia: np. `TIM4`.
4. Konfiguracja projektu:
   - Nazwa: `STM32Blink`.
   - IDE: `EwARM`.
5. Generowanie kodu.

### Krok 2: Konfiguracja PlatformIO

1. Utwórz nowy projekt w PlatformIO.
2. Skonfiguruj plik `platformio.ini`:

    ```ini
    [platformio]
    src_dir = ./
    include_dir = Core/Inc

    [env:genericSTM32F103C8]
    platform = ststm32
    board = genericSTM32F103C8
    framework = stm32cube
    lib_deps = 
        https://github.com/JAQUBA/JQBSTM32_Framework.git
    build_flags = 
        -DTASK_TIMER=htim4
    ```

## Autorzy

- **JAQUBA** - główny autor

## Kontakt

Jeśli masz pytania dotyczące projektu, możesz się skontaktować poprzez [profil GitHub](https://github.com/JAQUBA).

## Licencja

Ten projekt jest licencjonowany na zasadach GNU General Public License v3.0. Więcej informacji można znaleźć w pliku LICENSE dostępnym w tym repozytorium.
