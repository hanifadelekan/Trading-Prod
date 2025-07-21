import subprocess
import os
import sys
import threading

# The command you run in your terminal, broken into a list
COMMAND_TO_RUN = [
    os.path.expanduser("~/hl-visor"), # Expands ~ to the user's home directory
    "run-non-validator",
    "--write-raw-book-diffs",
    "--disable-output-file-buffering"
]

def stream_reader(stream, prefix):
    """
    Reads lines from a stream and prints them with a prefix.
    This function is designed to be run in a separate thread.
    """
    for line in iter(stream.readline, ''):
        print(f"[{prefix}] {line.strip()}")
    stream.close()

def main():
    """
    Runs the specified binary as a subprocess, captures its output and error streams
    in separate threads, and prints everything to the screen in real-time.
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

        # Create and start a thread to read from the subprocess's stdout
        stdout_thread = threading.Thread(
            target=stream_reader, 
            args=(process.stdout, "DATA") # "DATA" prefix for standard output
        )
        stdout_thread.daemon = True
        stdout_thread.start()

        # Create and start another thread to read from the subprocess's stderr
        stderr_thread = threading.Thread(
            target=stream_reader, 
            args=(process.stderr, "LOG") # "LOG" prefix for error output
        )
        stderr_thread.daemon = True
        stderr_thread.start()

        # Wait for the process to terminate. The threads will exit automatically.
        process.wait()
        
        # Wait for the reader threads to finish processing any remaining output
        stdout_thread.join()
        stderr_thread.join()

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
