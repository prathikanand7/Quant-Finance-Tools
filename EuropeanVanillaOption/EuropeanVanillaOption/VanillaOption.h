#pragma once

class VanillaOption {
 private:
  double K;      // Strike price
  double r;      // Risk-free interest rate
  double T;      // Maturity time; Expiry date
  double S;      // Underlying asset price
  double sigma;  // Volatility of the underlying asset

 public:
  VanillaOption();  // Default constructor
  VanillaOption(const double& _K, const double& _r, const double& _T,
                const double& _S, const double& _sigma);  // Constructor
  VanillaOption(const VanillaOption& option);             // Copy constructor
  VanillaOption& operator=(const VanillaOption& rhs);     // Assignment operator
  virtual ~VanillaOption();                               // Destructor
  void copy(const VanillaOption& option);                 // Copy method

  // Setters and Getter
  double GetK() const;
  void SetK(const double& _K);

  double Getr() const;
  void Setr(const double& _r);

  double GetT() const;
  void SetT(const double& _T);

  double GetS() const;
  void SetS(const double& _S);

  double Getsigma() const;
  void Setsigma(const double& _sigma);

  // Option price calling methods
  double CalculateCallPrice() const;
  double CalculatePutPrice() const;
};