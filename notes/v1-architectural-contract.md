1. Vehicle owns the simulated ECUs and vehicle-level context.

2. Each ECU owns its own state:
   - diagnostic session
   - DTCs
   - status
   - diagnostic data
   - diagnostic CAN addressing

3. CAN represents CAN frames and their movement between nodes.

4. ISO-TP turns complete diagnostic messages into CAN-sized
   transport units and reconstructs them in the other direction.

5. UDS understands diagnostic protocol semantics.
   It does not own ECU state.

6. UDS asks an ECU/domain object to perform semantic operations
   and translates domain results into UDS responses.

7. An application/orchestration layer connects the pieces without
   implementing CAN, ISO-TP, UDS, or ECU business rules itself.

8. Tester and ECU both conceptually sit above:
       UDS
        ↕
      ISO-TP
        ↕
       CAN

9. We build capabilities end-to-end.
   We do not complete whole isolated subsystems before integrating them.

10. Add abstractions only when the code demonstrates that we need them.