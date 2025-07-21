import asyncio
import websockets
import json
import subprocess
import os
import signal
import threading
from typing import Set, List

# Global state
active_clients: Set[websockets.WebSocketServerProtocol] = set()
child_processes: List[subprocess.Popen] = []
shutdown_timer: threading.Timer = None
timer_lock = threading.Lock()
is_monitoring_active = False # State variable to prevent duplicate processes

def run_command_in_background(cmd: str):
    """Run a command in the background in its own process group"""
    global child_processes
    try:
        # Use os.path.expanduser to handle '~' safely
        expanded_cmd = os.path.expanduser(cmd)
        
        # Create new process group, but DO NOT create pipes for stdout/stderr
        # to avoid deadlocks. The child's output will go to the server's console.
        process = subprocess.Popen(
            expanded_cmd,
            shell=True)
       
        child_processes.append(process)
        print(f"[*] Started command with PID: {process.pid}")
        return True
    except Exception as e:
        print(f"[!] Error starting command: {e}")
        return False

def terminate_child_processes():
    """Terminate all child processes and their groups"""
    global child_processes, is_monitoring_active
    
    print("[*] Terminating all child processes...")
    for process in child_processes:
        try:
            # Kill the entire process group
            os.killpg(os.getpgid(process.pid), signal.SIGTERM)
            print(f"[*] Sent SIGTERM to process group for PID {process.pid}")
        except ProcessLookupError:
            continue  # Process already dead
        except Exception as e:
            print(f"[!] Error terminating process {process.pid}: {e}")
    
    child_processes = []
    is_monitoring_active = False # Reset state
    print("[*] All child processes terminated")

def start_shutdown_timer():
    """Start a timer to terminate processes after 5 minutes of inactivity"""
    global shutdown_timer
    
    with timer_lock:
        if shutdown_timer and shutdown_timer.is_alive():
            shutdown_timer.cancel()
        
        # CORRECTED: This logic is now properly indented inside the function
        shutdown_timer = threading.Timer(300.0, terminate_child_processes)  # 5 minutes
        shutdown_timer.start()
        print("[*] Started 5-minute shutdown timer due to inactivity.")

def cancel_shutdown_timer():
    """Cancel the shutdown timer if it's running"""
    with timer_lock:
        if shutdown_timer and shutdown_timer.is_alive():
            shutdown_timer.cancel()
            print("[*] Active client connected. Cancelled shutdown timer.")

async def handler(websocket):
    global active_clients, is_monitoring_active

    active_clients.add(websocket)
    cancel_shutdown_timer()
    print(f"[+] Client connected. Total: {len(active_clients)}")

    try:
        async for message in websocket:
            try:
                data = json.loads(message)
                print(f"[📩] Received: {data}")

                if data.get('action') == 'start_stream':
                    if not is_monitoring_active:
                        print("[*] Starting monitoring stream...")
                        is_monitoring_active = True # Set state to active
                        
                        # Start child processes
                        run_command_in_background("./stripped_monitor hl/data/node_raw_book_diffs/hourly/20250717/23")
                        run_command_in_background("~/hl-visor run-non-validator --write-raw-book-diffs --disable-output-file-buffering")
                        
                        await websocket.send(json.dumps({"status": "OK", "message": "Monitoring started"}))
                    else:
                        print("[!] Monitoring is already active. Ignoring request.")
                        await websocket.send(json.dumps({"status": "WARN", "message": "Monitoring is already active"}))


                elif data.get('action') == 'stop_stream':
                    if is_monitoring_active:
                        print("[*] Stopping monitoring stream as requested...")
                        terminate_child_processes()
                        await websocket.send(json.dumps({"status": "OK", "message": "Monitoring stopped"}))
                    else:
                        print("[!] Monitoring is not active. Nothing to stop.")
                        await websocket.send(json.dumps({"status": "WARN", "message": "Monitoring is not active"}))
                else:
                    await websocket.send(json.dumps({"status": "ERROR", "message": "Unknown action"}))

            except json.JSONDecodeError:
                await websocket.send(json.dumps({"status": "ERROR", "message": "Invalid JSON"}))
            except Exception as e:
                print(f"[!!] An error occurred in the handler loop: {e}")
                await websocket.send(json.dumps({"status": "ERROR", "message": "An internal server error occurred"}))

    finally:
        active_clients.remove(websocket)
        print(f"[-] Client disconnected. Remaining: {len(active_clients)}")
        if not active_clients and is_monitoring_active:
            # Only start the shutdown timer if the processes are running
            start_shutdown_timer()

async def shutdown(sig, loop):
    """Graceful shutdown for the server and child processes"""
    print(f"\n[*] Caught signal {sig.name}. Shutting down...")
    terminate_child_processes()
    
    # Gracefully close websocket connections
    tasks = [asyncio.create_task(ws.close()) for ws in active_clients]
    await asyncio.gather(*tasks, return_exceptions=True)
    
    loop.stop()

async def main():
    """Start the WebSocket server"""
    loop = asyncio.get_running_loop()
    
    # Setup more robust signal handlers for graceful shutdown
    for sig in (signal.SIGINT, signal.SIGTERM):
        loop.add_signal_handler(sig, lambda s=sig: asyncio.create_task(shutdown(s, loop)))
    
    port = 9002
    async with websockets.serve(handler, "0.0.0.0", port, ping_interval=None):
        print(f"✅ Server listening on port {port}")
        print('[*] Send {"action":"start_stream"} to begin')
        await asyncio.Future()  # Run forever until shutdown is called

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except (KeyboardInterrupt, SystemExit):
        print("[*] Server has been shut down.")
