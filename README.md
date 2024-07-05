# JQBSTM32_Framework

## Opis projektu

JQBSTM32_Framework to framework ułatwiający rozwój aplikacji na mikrokontrolery STM32. Umożliwia szybkie i efektywne tworzenie projektów, zarządzanie zasobami i optymalizację kodu. Framework ten zawiera zestaw bibliotek i narzędzi, które wspierają różne aspekty programowania mikrokontrolerów STM32.

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
    - Upewnij się, że masz zainstalowane Visual Studio Code oraz rozszerzenie Platform.io.
    - Skonfiguruj STM32CubeMX do generowania kodu startowego dla Twojego projektu.

2. **Tworzenie nowego projektu**
    - Skorzystaj z dostarczonych skryptów i szablonów projektów, aby szybko zacząć pracę nad nowym projektem.

3. **Struktura katalogów**
    - Zasoby sprzętowe trzymane są w katalogu `Hardware`.
    - Biblioteki do obsługi peryferiów znajdują się w katalogu `Utils`.

## Wymagania

- **System operacyjny**: Windows, macOS, Linux
- **Narzędzia**:
    - STM32CubeMX
    - Visual Studio Code z rozszerzeniem Platform.io
    - Kompilator ARM (np. arm-none-eabi-gcc)
    - Narzędzia do debugowania (np. OpenOCD, ST-Link)
- **Biblioteki**:
    - CMSIS
    - STM32 HAL

## Autorzy

- **JAQUBA** - główny autor

## Kontakt

Jeśli masz pytania dotyczące projektu, możesz się skontaktować poprzez [profil GitHub](https://github.com/JAQUBA).

## Dodatkowe informacje

![Status projektu](https://img.shields.io/badge/status-w%20rozwoju-yellow)
![Licencja](https://img.shields.io/badge/licencja-GPL-blue)

JQBSTM32 Framework jest frameworkiem do programowania mikrokontrolerów STM32, rozwijanym przez JAQUBA. Projekt jest nadal w fazie rozwoju.

## Użycie

1. **STM32CubeMX**: Projekt został skonfigurowany za pomocą STM32CubeMX. Można go dalej dostosować przy użyciu tego narzędzia.

2. **Visual Studio Code i PlatformIO**: Do kompilacji i wgrywania oprogramowania na urządzenie wymagane jest środowisko Visual Studio Code z wtyczką PlatformIO.

## Struktura projektu

- **Hardware/**: Zawiera pliki konfiguracyjne sprzętowe używane przez STM32CubeMX.
  
- **Utils/**: Biblioteki i narzędzia do obsługi peryferiów mikrokontrolera.

## Licencja

Ten projekt jest licencjonowany na zasadach GNU General Public License v3.0. Więcej informacji można znaleźć w pliku LICENSE dostępnym w tym repozytorium.

