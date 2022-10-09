const { expect } = require("chai")
const { ethers, upgrades, waffle } = require("hardhat")

const BN = ethers.BigNumber.from

describe('EarlyBirdCollateral contract', function () {

    let owner, authorized, user, user2, user3, h_user, h_user2, h_user3
    let usdt, vault
    let decimals

    const toDecimal = function(value) {
        return BN(value).mul(BN(10).pow(decimals))
    }

    before(async () => {
        const [_owner, _authorized, _user, _user2, _user3, _h_user, _h_user2, _h_user3] = await ethers.getSigners()
        owner = _owner
        authorized = _authorized
        user = _user
        user2 = _user2
        user3 = _user3
        h_user = _h_user
        h_user2 = _h_user2
        h_user3 = _h_user3

        const USDT = await ethers.getContractFactory('TestERC20')
        usdt = await USDT.deploy()
        await usdt.deployed()
        
        decimals = await usdt.decimals()

        await usdt.transfer(user.address, toDecimal(200000))
        await usdt.transfer(user2.address, toDecimal(400000))
        await usdt.transfer(user3.address, toDecimal(15000000))

        const Vault = await ethers.getContractFactory("EarlyBirdCollateral")
        vault = await Vault.deploy(usdt.address)
        await vault.deployed()
    })

    it('setAuthorized', async () => {
        await vault.setAuthorized(authorized.address, true)
    })

    it('setQuota', async () => {
        await vault.connect(authorized).setQuota(user.address, h_user.address, toDecimal(200000))
        expect(await vault.quota(user.address)).to.equal(toDecimal(200000))

        await vault.connect(authorized).setQuota(user2.address, h_user2.address, toDecimal(200000))
        expect(await vault.quota(user2.address)).to.equal(toDecimal(200000))

        expect(vault.connect(authorized).setQuota(user3.address, h_user3.address, toDecimal(0))).to.be.revertedWith('Quota amount should be greater than 0.')
        expect(vault.connect(authorized).setQuota(user.address, h_user3.address, toDecimal(200000))).to.be.revertedWith('The Ethereum address is taken.')
        expect(vault.connect(authorized).setQuota(user3.address, h_user.address, toDecimal(200000))).to.be.revertedWith('The Huygens address is taken.')
        
        await vault.connect(authorized).setQuota(user3.address, h_user3.address, toDecimal(15000000))
        expect(await vault.quota(user3.address)).to.equal(toDecimal(15000000))
    })

    it('deposit-1', async () => {
        await usdt.connect(user).approve(vault.address, toDecimal(200000))
        await vault.connect(user).deposit(toDecimal(200000))
        expect(await usdt.balanceOf(vault.address)).to.equal(toDecimal(200000))
    })

    it('deposit-2', async () => {
        await usdt.connect(user2).approve(vault.address, toDecimal(400000))
        await vault.connect(user2).deposit(toDecimal(200000))
        expect(await usdt.balanceOf(vault.address)).to.equal(toDecimal(400000))

        expect(vault.connect(user2).deposit(toDecimal(200000))).to.be.revertedWith('Deposit amount doesn\'t match quota.')
    })

    it('deposit-3', async () => {
        await usdt.connect(user3).approve(vault.address, toDecimal(15000000))

        expect(vault.connect(user3).deposit(toDecimal(0))).to.be.revertedWith('Deposit amount should be greater than 0.')
        expect(vault.connect(user3).deposit(toDecimal(100000))).to.be.revertedWith('Deposit amount doesn\'t match quota.')
        expect(vault.connect(user3).deposit(toDecimal(14600001))).to.be.revertedWith('Total deposit balance cannot exceed 15 million.')

        await vault.connect(user3).deposit(toDecimal(14600000))
        expect(await usdt.balanceOf(vault.address)).to.equal(toDecimal(15000000))
    })

    it('totalBalance', async () => {
        expect(await usdt.balanceOf(vault.address)).to.equal(await vault.totalBalance())
        expect(await vault.totalBalance()).to.equal(toDecimal(15000000))
    })

    it('refund', async () => {
        await vault.connect(owner).refund(user.address, 2000000)
        expect(await usdt.balanceOf(user.address)).to.equal(66666800000)
        expect(await usdt.balanceOf(vault.address)).to.equal(14933333200000)

        await vault.connect(owner).refund(user2.address, 1500000)
        expect(await usdt.balanceOf(user2.address)).to.equal(300000000000)
        expect(await usdt.balanceOf(vault.address)).to.equal(14833333200000)

        await vault.connect(owner).refund(user3.address, 500000)
        expect(await usdt.balanceOf(user3.address)).to.equal(12566676400000)
        expect(await usdt.balanceOf(vault.address)).to.equal(2666656800000)
    })

    it('balances', async () => {
        expect(await vault.balances(user.address)).to.equal(133333200000)
        expect(await vault.balances(user2.address)).to.equal(100000000000)
        expect(await vault.balances(user3.address)).to.equal(2433323600000)
    })

    it('userRefunded', async () => {
        expect(await vault.userRefunded(user.address)).to.equal(true)
        expect(await vault.userRefunded(user2.address)).to.equal(true)
        expect(await vault.userRefunded(user3.address)).to.equal(true)
    })

    it('deposit directly', async () => {
        await usdt.connect(user3).transfer(vault.address, toDecimal(30000))
        expect(await usdt.balanceOf(vault.address)).to.equal(2696656800000)
    })

    it('refund again', async () => {
        expect(vault.connect(owner).refund(user.address, 1)).to.be.revertedWith('Already refunded')
    })

    it('withraw', async () => {
        await vault.connect(owner).withraw(toDecimal(100000))
        expect(await vault.totalBalance()).to.equal(2596656800000)
    })

    it('withrawAll', async () => {
        await vault.connect(owner).withrawAll()
        expect(await vault.totalBalance()).to.equal(0)
    })
})