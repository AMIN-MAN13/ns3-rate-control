# Comparison of Various Rate Control Algorithms in Wi-Fi (IEEE 802.11ac)

This project focuses on comparing and analyzing various rate control algorithms used in Wi-Fi systems based on the IEEE 802.11ac standard. It evaluates their performance in terms of throughput, packet loss, and efficiency under different conditions.

---

## Overview

The project addresses the following key objectives:
- Evaluate the performance of rate control algorithms in IEEE 802.11ac networks.
- Compare throughput, packet loss, and efficiency at varying distances.
- Identify the optimal rate control algorithm for dynamic network conditions.

---

## Key Features

- **Rate Control Algorithms Studied**:
  - **Constant Rate**: Maintains a fixed transmission rate.
  - **IdealWifiManager**: Dynamically adapts the rate based on real-time feedback.
  - **Minstrel-HT**: Utilizes historical performance data for rate selection.
  - **Thompson Sampling**: Bayesian-based algorithm for rate optimization.

- **Simulation Environment**:
  - Built using the **ns-3** simulator.
  - Distances range from 20m to 380m.
  - Metrics analyzed: Throughput, packet loss, and standard deviation.

---

## Results

### Throughput:
- **IdealWifiManager** and **Thompson Sampling** outperform others at short distances.
- **Minstrel-HT** performs well in long-range scenarios.

### Packet Loss:
- **IdealWifiManager** exhibits minimal packet loss due to dynamic feedback.
- **Minstrel-HT** maintains consistent performance in extended ranges.

For detailed graphs and analysis, refer to the report in the repository.

---

## Prerequisites

To run the simulations and analyze the results, ensure the following tools are installed:

- **ns-3**: For Wi-Fi simulation.
- **Python**: For data visualization and analysis.
- **C++**: For implementing and modifying simulation scripts.

---
