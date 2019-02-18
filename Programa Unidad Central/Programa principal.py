# -----------------------------------------------------------------------------
# IMPORTS
# -----------------------------------------------------------------------------
import StateMachine as StateMachine
import time

stateMachine = StateMachine.StateMachine()
while True:
    stateMachine.StartStateMachine()
    time.sleep(0.5)
