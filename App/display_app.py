import sys

def main():
    print("ENTERED PYTHON APP\r\n")
    for line in sys.stdin:
        print("PY:" + line)

if __name__ == "__main__":
    main()