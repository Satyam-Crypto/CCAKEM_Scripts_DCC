# Security Evaluation for CCAKEM

This directory contains all scripts and notebooks used for evaluating the security of the proposed **CCAKEM** scheme. The evaluation consists of:

- Decryption Failure Probability (DFP)
- Decryption Failure Attack (DFA) complexity
- Core-SVP hardness estimation
- MLWE security estimation (in terms of Gate count)

## Directory Structure

```
SecurityAnalysis/
├── DFP_CCAKEM.ipynb
├── DFP_Attack_CCAKEM.ipynb
├── lattice-estimator/
│   └── Core-SVP_Hardness_CCAKEM.ipynb
└── leaky-LWE-Estimator/
    └── framework/
        └── MLWE_Estimate_CCAKEM.ipynb
```

---

## 1. Decryption Failure Probability

**Notebook**

```
./DFP_CCAKEM.ipynb
```

This notebook computes the **Decryption Failure Probability (DFP)** for all CCAKEM parameter sets.

**Output**

- Decryption failure probability
---

## 2. Decryption Failure Attack Complexity

**Notebook**

```
./DFP_Attack_CCAKEM.ipynb
```

This notebook estimates the computational complexity of decryption-failure attacks using the DFP values obtained from `DFP_CCAKEM.ipynb`.

It estimates both classical and quantum attack cost for all parameter sets for some of the scale values, `\alpha`.


**Output**

- Classical and quantum DFA costs on a log₂ scale, for each parameter set.

---

## 3. Core-SVP Hardness Estimation

**Notebook**

```
./lattice-estimator/Core-SVP_Hardness_CCAKEM.ipynb
```

This notebook uses the official **Lattice Estimator** framework to estimate the hardness of the underlying LWE problem.

It computes:

- Classical Core-SVP security
- Quantum Core-SVP security

for

- CCAKEM-512
- CCAKEM-768
- CCAKEM-1024

**Note**

All remaining files under

```
./lattice-estimator/
```

belong to the original Lattice Estimator framework and are required for executing the notebook.

---

## 4. MLWE Security Estimation

**Notebook**

```
./leaky-LWE-Estimator/framework/MLWE_Estimate_CCAKEM.ipynb
```

This notebook uses the **Leaky-LWE Estimator** framework to estimate the hardness of the Module-LWE problem.

It computes:

- MLWE attack cost
- BKZ block size
- Memory complexity
- Gate complexity

for

- CCAKEM-512
- CCAKEM-768
- CCAKEM-1024

**Note**

All remaining files under

```
./leaky-LWE-Estimator/
```

belong to the original Leaky-LWE Estimator framework and are required for executing the notebook.

---

# Execution Order

To reproduce the security evaluation, execute the notebooks in the following order:

1. `./DFP_CCAKEM.ipynb`
2. `./DFP_Attack_CCAKEM.ipynb`
3. `./lattice-estimator/Core-SVP_Hardness_CCAKEM.ipynb`
4. `./leaky-LWE-Estimator/framework/MLWE_Estimate_CCAKEM.ipynb`

---

# Requirements
- Numpy
- Python 3.x
- Jupyter Notebook
- SageMath 10.3 (required for `./leaky-LWE-Estimator/framework/MLWE_Estimate_CCAKEM.ipynb` and `./lattice-estimator/Core-SVP_Hardness_CCAKEM.ipynb`)
- All dependencies included with the Lattice Estimator and Leaky-LWE Estimator frameworks