import chess
import subprocess
from time import sleep
import re


def start_engine(engine_path, init_command):
    """Start the chess engine and init it"""
    engine = subprocess.Popen(
        ["stdbuf", "-oL", engine_path],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        text=True,
    )
    engine.stdin.write(init_command + "\n")
    engine.stdin.flush()
    return engine


def get_move(engine):
    move_pattern = re.compile(r"^[a-h][1-8][a-h][1-8](?:[qrbn])?$")
    while True:
        output = engine.stdout.readline().strip()
        if move_pattern.match(output):
            return output


def send_move(engine, move):
    engine.stdin.write(move + "\n")
    engine.stdin.flush()


def main():
    commit = input("commit: ")

    subprocess.call(
        "git clone https://github.com/Thorvicnet/Robespierre tmp/".split(" ")
    )
    subprocess.call(f"git checkout {commit}".split(" "), cwd="tmp/")
    subprocess.call("make", cwd="tmp/")
    subprocess.call("make")

    engine1_path = "tmp/bin/main"
    engine2_path = "bin/main"

    engine1 = start_engine(engine1_path, "initbp")
    engine2 = start_engine(engine2_path, "initpb")

    board = chess.Board()

    print("Starting the game\n")

    current_engine = engine1
    other_engine = engine2

    while not board.is_game_over():
        print(board)
        print()

        move = get_move(current_engine)
        print(f"Move from engine: {move}")

        try:
            board.push_uci(move)
        except ValueError:
            print(f"Invalid move: {move}")
            break

        send_move(other_engine, move)
        current_engine, other_engine = other_engine, current_engine

        sleep(1)

    print("Game over")
    print("Result:", board.result())

    engine1.terminate()
    engine2.terminate()

    subprocess.call("rm -rf tmp/".split(" "))


if __name__ == "__main__":
    main()
