Import("env")

def print_hex_creation_info(source, target, env):
    print("=" * 50)
    print("TWORZENIE PLIKU HEX")
    print("=" * 50)
    print(f"Źródło (ELF): {source[0]}")
    print(f"Cel (HEX): {target[0]}")
    print(f"Narzędzie OBJCOPY: {env['OBJCOPY']}")
    print("Parametry konwersji:")
    print("  -O ihex     : Format wyjściowy Intel HEX")
    print("  -R .eeprom : Usunięcie sekcji EEPROM")
    print("Rozpoczynam konwersję ELF -> HEX...")
    print("-" * 50)

env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    [
        env.VerboseAction(print_hex_creation_info, "Przygotowanie do tworzenia pliku HEX"),
        env.VerboseAction(" ".join([
            "$OBJCOPY", "-O", "ihex", "-R", ".eeprom",
            "$BUILD_DIR/${PROGNAME}.elf", "$BUILD_DIR/${PROGNAME}.hex"
        ]), "Tworzenie $BUILD_DIR/${PROGNAME}.hex"),
        env.VerboseAction(lambda source, target, env: print("✓ Plik HEX został pomyślnie utworzony!"), "Zakończenie tworzenia pliku HEX")
    ]
)