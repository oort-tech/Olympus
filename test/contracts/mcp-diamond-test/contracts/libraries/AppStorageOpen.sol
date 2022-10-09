// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import { IBEP20 } from "../util/IBEP20.sol";

// =========== TokenList structs ===========
struct MarketData {
    bytes32 market;
    address tokenAddress;
    uint256 decimals;
    uint256 chainId;
    uint256 minAmount;
}

// =========== Comptroller structs ===========
/// @notice each APY or APR struct holds the recorded changes in interest data & the
/// corresponding time for a particular commitment type.
struct APY {
    bytes32 commitment;
    uint256[] time; // ledger of time when the APY changes were made.
    uint256[] apyChanges; // ledger of APY changes.
}

struct APR {
    bytes32 commitment; // validity
    uint256[] time; // ledger of time when the APR changes were made.
    uint256[] aprChanges; // Per block.timestamp APR is tabulated in here.
}

// =========== Deposit structs ===========
struct SavingsAccount {
    uint256 accOpenTime;
    address account;
    DepositRecords[] deposits;
    YieldLedger[] yield;
}

struct DepositRecords {
    uint256 id;
    bytes32 market;
    bytes32 commitment;
    uint256 amount;
    uint256 createdAt;
    bool isTimelockApplicable; // is timelockApplicalbe or not. Except the flexible deposits, the timelock is applicabel on all the deposits.
    bool isTimelockActivated; // is timelockApplicalbe or not. Except the flexible deposits, the timelock is applicabel on all the deposits.
    uint256 timelockValidity;
    uint256 activationTime; // block.timestamp(isTimelockActivated) + timelockValidity.
}

struct YieldLedger {
    uint256 id;
    bytes32 market; //_market this yield is calculated for
    uint256 oldLengthAccruedYield; // length of the APY time array.
    uint256 oldTime; // last recorded block num. This is when this struct is lastly updated.
    uint256 accruedYield; // accruedYield in
}

struct ActiveDeposits {
    uint256[] id;
    bytes32[] market;
    bytes32[] commitment;
    uint256[] amount;
    uint256[] savingsInterest;
}
// =========== Loan structs ===========

enum STATE {
    ACTIVE,
    REPAID
}

struct LoanAccount {
    uint256 accOpenTime;
    address account;
    LoanRecords[] loans; // 2 types of loans. 3 markets intially. So, a maximum o f6 records.
    CollateralRecords[] collaterals;
    DeductibleInterest[] accruedAPR;
    LoanState[] loanState;
}
struct LoanRecords {
    uint256 id;
    bytes32 market;
    bytes32 commitment;
    uint256 amount;
    bool isSwapped; //true or false. Update when a loan is swapped
    uint256 createdAt; // block.timestamp
    address owner;
    uint256 activationTime; // block.timestamp when yield withdrawal request was placed.
}

struct LoanState {
    uint256 id; // loan.id
    bytes32 loanMarket;
    uint256 actualLoanAmount;
    bytes32 currentMarket;
    uint256 currentAmount;
    STATE state;
}
struct CollateralRecords {
    uint256 id;
    bytes32 market;
    bytes32 commitment;
    uint256 initialAmount;
    uint256 amount;
    uint256 timelockValidity;
    bool isTimelockActivated; // timelock duration
    uint256 activationTime; // blocknumber when yield withdrawal request was placed.
}

// DeductibleInterest stores the amount_ of interest deducted.
struct DeductibleInterest {
    uint256 id; // Id of the loan the interest is being deducted for.
    bytes32 market; // market_ this yield is calculated for
    uint256 oldLengthAccruedInterest; // length of the APY time array.
    uint256 oldTime; // length of the APY time array.
    uint256 accruedInterest;
}

struct ActiveLoans {
    bytes32[] loanMarket;
    bytes32[] loanCommitment;
    uint256[] loanAmount;
    bytes32[] collateralMarket;
    uint256[] collateralAmount;
    bool[] isSwapped;
    bytes32[] loanCurrentMarket;
    uint256[] loanCurrentAmount;
    uint256[] borrowInterest;
    STATE[] state;
}

// =========== OracleOpen structs =============
struct PriceData {
    bytes32 market;
    uint256 amount;
    uint256 price;
}

// =========== AccessRegistry structs =============

struct AppStorageOpen {
    IBEP20 token;
    mapping(bytes4 => uint256) facetIndex;
    address reserveAddress;
    // ===========  admin addresses ===========
    bytes32 superAdmin; // superAdmin address backed in function setupgradeAdmin()
    address superAdminAddress; // Address of AccessRegistry
    address upgradeAdmin;
    // =========== TokenList state variables ===========
    bytes32 adminTokenList;
    bytes32[] pMarkets; // Primary markets
    bytes32[] sMarkets; // Secondary markets
    mapping(bytes32 => bool) tokenSupportCheck;
    mapping(bytes32 => uint256) marketIndex;
    mapping(bytes32 => MarketData) indMarketData;
    mapping(bytes32 => bool) token2SupportCheck;
    mapping(bytes32 => uint256) market2Index;
    mapping(bytes32 => MarketData) indMarket2Data;
    // =========== Comptroller state variables ===========
    bytes32 adminComptroller;
    bytes32[] depositCommitment;
    bytes32[] borrowCommitment;
    uint256 reserveFactor;
    uint256 timelockValidity; // timelock duration
    uint256 loanIssuanceFees;
    uint256 loanClosureFees;
    uint256 loanPreClosureFees;
    uint256 collateralPreClosureFees;
    uint256 depositPreClosureFees;
    uint256 maxWithdrawalFactor;
    uint256 maxWithdrawalBlockLimit;
    uint256 depositWithdrawalFees;
    uint256 collateralReleaseFees;
    uint256 yieldConversionFees;
    uint256 marketSwapFees;
    mapping(bytes32 => mapping(bytes32 => APY)) indAPYRecords;
    mapping(bytes32 => mapping(bytes32 => APR)) indAPRRecords;
    // =========== Liquidator state variables ===========
    bytes32 adminLiquidator;
    bytes32 protocolOwnedLiquidator;
    // =========== DynamicInterest state variables ======
    bytes32 adminDynamicInterest;
    uint256[] borrowInterests;
    uint256[] depositInterests;
    uint256[] interestFactors;
    mapping(bytes32 => uint256) commitmentDays;
    // =========== Deposit state variables ===========
    bytes32 adminDeposit;
    mapping(address => SavingsAccount) savingsPassbook; // Maps an account to its savings Passbook
    mapping(address => mapping(bytes32 => mapping(bytes32 => DepositRecords))) indDepositRecord; // address =>_market => _commitment => depositRecord
    mapping(address => mapping(bytes32 => mapping(bytes32 => YieldLedger))) indYieldRecord; // address =>_market => _commitment => depositRecord
    mapping(address => ActiveDeposits) getActiveDeposits;
    //  Balance monitoring  - Deposits
    mapping(bytes32 => uint256) marketReservesDeposit; // mapping(market => marketBalance)
    mapping(bytes32 => uint256) marketUtilisationDeposit; // mapping(market => marketBalance)
    // =========== OracleOpen state variables ==============
    bytes32 adminOpenOracle;
    mapping(bytes32 => address) pairAddresses;
    PriceData[] prices;
    mapping(uint256 => PriceData) priceData;
    uint256 requestEventId;
    // =========== Loan state variables ============
    bytes32 adminLoan;
    bytes32 adminLoan1;
    bytes32 adminLoan2;
    IBEP20 loanToken;
    IBEP20 withdrawToken;
    IBEP20 collateralToken;
    // STRUCT Mapping
    mapping(address => LoanAccount) loanPassbook;
    mapping(address => mapping(bytes32 => mapping(bytes32 => LoanRecords))) indLoanRecords;
    mapping(address => mapping(bytes32 => mapping(bytes32 => CollateralRecords))) indCollateralRecords;
    mapping(address => mapping(bytes32 => mapping(bytes32 => DeductibleInterest))) indAccruedAPR;
    mapping(address => mapping(bytes32 => mapping(bytes32 => LoanState))) indLoanState;
    mapping(address => ActiveLoans) getActiveLoans;
    //  Balance monitoring  - Loan
    mapping(bytes32 => uint256) marketReservesLoan; // mapping(market => marketBalance)
    mapping(bytes32 => uint256) marketUtilisationLoan; // mapping(market => marketBalance)
    address[] borrowers;
    // =========== Reserve state variables ===========
    bytes32 adminReserve;

    // =========== AccessRegistry state variables ==============
}
