import subprocess
import os
import sys

# The command you run in your terminal, broken into a list
COMMAND_TO_RUN = [
    os.path.expanduser("~/hl-visor"), # Expands ~ to the user's home directory
    "run-non-validator",
    "--write-raw-book-diffs",
    "--disable-output-file-buffering"
]

def main():
    """
    Runs the specified binary as a subprocess, captures its output via a pipe,
    and prints it to the screen in real-time.
    """
    print(f"[Wrapper] Starting command: {' '.join(COMMAND_TO_RUN)}")
    
    try:
        # Start the subprocess.
        # - stdout=subprocess.PIPE: Redirects the binary's output to a pipe.
        # - stderr=subprocess.PIPE: Redirects error output to a pipe.
        # - text=True: Decodes stdout/stderr as text using the default encoding.
        # - bufsize=1: Sets line-buffering, so we get output line-by-line.
        process = subprocess.Popen(
            COMMAND_TO_RUN,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1 
        )

        # Use iter() to read from the process's stdout line by line until it closes.
        # This is a blocking call, but it yields a line as soon as it's available.
        for line in iter(process.stdout.readline, ''):
            # Print the captured output to this script's stdout.
            # .strip() removes leading/trailing whitespace, including the newline character.
            print(line.strip())

        # After the main loop finishes, check for any error output.
        stderr_output = process.stderr.read()
        if stderr_output:
            print(f"\n[Wrapper] Error output from subprocess:\n{stderr_output.strip()}", file=sys.stderr)

        # Wait for the process to terminate and get its exit code.
        process.wait()
        print(f"\n[Wrapper] Subprocess finished with exit code: {process.returncode}")

    except FileNotFoundError:
        print(f"[Wrapper ERROR] Command not found: {COMMAND_TO_RUN[0]}", file=sys.stderr)
        print("Please ensure 'hl-visor' is in your home directory and is executable (chmod +x ~/hl-visor).", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"[Wrapper ERROR] An unexpected error occurred: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n[Wrapper] Script interrupted by user. Exiting.")
