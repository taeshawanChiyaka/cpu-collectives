# Project visuals

The project includes editable, scalable SVG illustrations for the README and documentation:

- [All-reduce flow](images/all-reduce-flow.svg) — communicator stages rank inputs, shows the explicit baseline and parallel chunk algorithms, and returns the result to all ranks.
- [Benchmark latency chart](images/benchmark-latency.svg) — compares the median of nine reported runs for the two algorithms at four payload sizes and two participant counts. These are contributor-reported, hardware-specific measurements; read [the benchmark notes](benchmark-results.md) before drawing conclusions.

To display either graphic in Markdown:

```md
![All-reduce flow](docs/images/all-reduce-flow.svg)
![Reported benchmark latency](docs/images/benchmark-latency.svg)
```

SVG is used so the illustrations remain crisp at different screen sizes and can be edited with a text editor or vector graphics tool.
