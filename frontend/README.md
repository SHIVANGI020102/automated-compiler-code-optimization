# Compiler Optimizer Frontend

This is a lightweight HTML/CSS/JavaScript visualization layer for the existing C compiler optimizer.

## Run on macOS

From the frontend folder:

    python3 -m http.server 8080

Then open localhost:8080 in your browser.

You may also open index.html directly.

## Important

The browser has a small interactive demonstration for constant folding, algebraic simplification and simple common-subexpression elimination. It is not a replacement for the C optimization engine.

Use the C executable as the authoritative implementation for the complete optimization pipeline, 20-case differential verification, cost evaluation, dataset processing, and CSV metrics.

A future connected version can expose the C executable through a local HTTP backend and have this frontend call it.
