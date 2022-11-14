"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const chai_1 = require("chai");
const abiParser_1 = require("./abiParser");
const errors_1 = require("../utils/errors");
const typeParser_1 = require("./typeParser");
describe("extractAbi", () => {
    it("should throw error on not JSON ABI", () => {
        const inputJson = `abc`;
        chai_1.expect(() => abiParser_1.extractAbi(inputJson)).to.throw(errors_1.MalformedAbiError, "Not a json");
    });
    it("should throw error on malformed ABI", () => {
        const inputJson = `{ "someProps": "abc" }`;
        chai_1.expect(() => abiParser_1.extractAbi(inputJson)).to.throw(errors_1.MalformedAbiError, "Not a valid ABI");
    });
    it("should work with simple abi", () => {
        const inputJson = `[{ "name": "piece" }]`;
        chai_1.expect(abiParser_1.extractAbi(inputJson)).to.be.deep.eq([{ name: "piece" }]);
    });
    it("should work with nested abi (truffle style)", () => {
        const inputJson = `{ "abi": [{ "name": "piece" }] }`;
        chai_1.expect(abiParser_1.extractAbi(inputJson)).to.be.deep.eq([{ name: "piece" }]);
    });
});
describe("parseEvent", () => {
    it("should work", () => {
        const expectedEvent = {
            anonymous: false,
            inputs: [
                { indexed: true, name: "_from", type: "address" },
                { indexed: false, name: "_value", type: "uint256" },
            ],
            name: "Deposit",
            type: "event",
        };
        const parsedEvent = abiParser_1.parseEvent(expectedEvent);
        chai_1.expect(parsedEvent).to.be.deep.eq({
            name: "Deposit",
            inputs: [
                { name: "_from", isIndexed: true, type: new typeParser_1.AddressType() },
                { name: "_value", isIndexed: false, type: new typeParser_1.UnsignedIntegerType(256) },
            ],
        });
    });
});
