/*
 * Copyright (c) 2019 Xilinx Inc.
 * Written by Francisco Iglesias.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <sstream>
#include <string>
#include <vector>
#include <array>

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

#include "traffic-generators/tg-tlm.h"
#include "checkers/pc-ace.h"
#include "checkers/config-ace.h"
#include "test-modules/memory.h"
#include "test-modules/signals-ace.h"
#include "siggen-ace.h"

#define AXI_ADDR_WIDTH 64
#define AXI_DATA_WIDTH 64

typedef ACESignals<
	AXI_ADDR_WIDTH,	// ADDR_WIDTH
	AXI_DATA_WIDTH	// DATA_WIDTH
> ACESignals__;

SIGGEN_TESTSUITE(TestSuite)
{
	SIGGEN_TESTSUITE_CTOR(TestSuite)
	{}

	void run_tests()
	{
		SetMessageType(AXI_HANDSHAKE_ERROR);
		SetMessageType(ACE_HANDSHAKE_ERROR);

		TESTCASE(test_ar_rr_channel_handshakes);
		TESTCASE(test_aw_w_b_channel_handshakes);
		TESTCASE(test_multiple_ar_rr_handshakes);
		TESTCASE(test_multiple_aw_w_b_channel_handshakes);

		TESTCASE(test_ac_cr_channel_handshakes);
		TESTCASE(test_ac_cr_cd_channel_handshakes);

		TESTCASE_NEG(test_ar_ch_no_arready);
		TESTCASE_NEG(test_ar_rr_ch_no_rvalid);
		TESTCASE_NEG(test_aw_ch_no_awready);
		TESTCASE_NEG(test_aw_w_ch_no_wvalid);
		TESTCASE_NEG(test_aw_w_b_ch_no_bvalid);

		TESTCASE_NEG(test_arvalid_toggle_before_arready);

		TESTCASE_NEG(test_ar_rr_no_rack);
		TESTCASE_NEG(test_aw_w_b_no_wack);

		TESTCASE_NEG(test_ac_no_cr);
		TESTCASE_NEG(test_ac_cr_no_cd);
	}

	void test_ar_rr_channel_handshakes()
	{
		arvalid.write(true);
		arready.write(true);

		wait(clk.posedge_event());

		arvalid.write(false);
		arready.write(false);

		rvalid.write(true);
		rready.write(true);
		rlast.write(true);

		wait(clk.posedge_event());

		rvalid.write(false);
		rready.write(false);
		rlast.write(false);

		rack.write(true);

		wait(clk.posedge_event());

		rack.write(false);

		wait(clk.posedge_event());
	}

	void test_aw_w_b_channel_handshakes()
	{
		awvalid.write(true);
		awready.write(true);

		wait(clk.posedge_event());

		awvalid.write(false);
		awready.write(false);

		wvalid.write(true);
		wready.write(true);
		wlast.write(true);

		wait(clk.posedge_event());

		wvalid.write(false);
		wready.write(false);
		wlast.write(false);

		bvalid.write(true);
		bready.write(true);

		wait(clk.posedge_event());

		bvalid.write(false);
		bready.write(false);

		wack.write(true);

		wait(clk.posedge_event());

		wack.write(false);

		wait(clk.posedge_event());
	}

	void test_ac_cr_channel_handshakes()
	{
		acvalid.write(true);
		acready.write(true);

		wait(clk.posedge_event());

		acvalid.write(false);
		acready.write(false);

		crvalid.write(true);
		crready.write(true);

		wait(clk.posedge_event());

		crvalid.write(false);
		crready.write(false);

		wait(clk.posedge_event());
	}

	void test_ac_cr_cd_channel_handshakes()
	{
		acvalid.write(true);
		acready.write(true);

		wait(clk.posedge_event());

		acvalid.write(false);
		acready.write(false);

		crvalid.write(true);
		crready.write(true);
		crresp.write(1);

		wait(clk.posedge_event());

		crvalid.write(false);
		crready.write(false);
		crresp.write(0);

		cdvalid.write(true);
		cdready.write(true);

		for (int i = 0; i < 8; i++) {
			wait(clk.posedge_event());
		}

		cdvalid.write(false);
		cdready.write(false);

		wait(clk.posedge_event());
	}

	void test_multiple_ar_rr_handshakes()
	{
		arvalid.write(true);
		arready.write(true);

		wait(clk.posedge_event());

		rvalid.write(true);
		rready.write(true);
		rlast.write(true);

		wait(clk.posedge_event());

		rack.write(true);

		wait(clk.posedge_event());
		wait(clk.posedge_event());
		wait(clk.posedge_event());
		wait(clk.posedge_event());

		arvalid.write(false);
		arready.write(false);

		rvalid.write(true);
		rready.write(true);
		rlast.write(true);

		wait(clk.posedge_event());

		rvalid.write(false);
		rready.write(false);
		rlast.write(false);

		wait(clk.posedge_event());

		rack.write(false);

		wait(clk.posedge_event());
	}

	void test_multiple_aw_w_b_channel_handshakes()
	{
		awvalid.write(true);
		awready.write(true);

		wait(clk.posedge_event());

		wvalid.write(true);
		wready.write(true);
		wlast.write(true);

		wait(clk.posedge_event());

		bvalid.write(true);
		bready.write(true);

		wait(clk.posedge_event());

		wack.write(true);

		wait(clk.posedge_event());
		wait(clk.posedge_event());
		wait(clk.posedge_event());
		wait(clk.posedge_event());

		awvalid.write(false);
		awready.write(false);

		wait(clk.posedge_event());

		wvalid.write(false);
		wready.write(false);
		wlast.write(false);

		wait(clk.posedge_event());

		bvalid.write(false);
		bready.write(false);

		wait(clk.posedge_event());

		wack.write(false);

		wait(clk.posedge_event());
	}

	void test_ar_ch_no_arready()
	{
		arvalid.write(true);

		for (int i = 0; i < 200; i++) {
			wait(clk.posedge_event());
		}

		// Clean up
		arready.write(true);

		wait(clk.posedge_event());

		arvalid.write(false);
		arready.write(false);

		rvalid.write(true);
		rready.write(true);

		wait(clk.posedge_event());

		rvalid.write(false);
		rready.write(false);

		rack.write(true);

		wait(clk.posedge_event());

		rack.write(false);

		wait(clk.posedge_event());
	}

	void test_ar_rr_ch_no_rvalid()
	{
		arvalid.write(true);
		arready.write(true);

		wait(clk.posedge_event());

		arvalid.write(false);
		arready.write(false);

		for (int i = 0; i < 200; i++) {
			wait(clk.posedge_event());
		}

		// Clean up
		rvalid.write(true);
		rready.write(true);

		wait(clk.posedge_event());

		rvalid.write(false);
		rready.write(false);

		wait(clk.posedge_event());

		rack.write(true);

		wait(clk.posedge_event());

		rack.write(false);

		wait(clk.posedge_event());
	}

	void test_aw_ch_no_awready()
	{
		awvalid.write(true);

		for (int i = 0; i < 200; i++) {
			wait(clk.posedge_event());
		}

		// Clean up
		awready.write(true);

		wait(clk.posedge_event());

		awvalid.write(false);
		awready.write(false);

		wvalid.write(true);
		wready.write(true);
		wlast.write(true);

		wait(clk.posedge_event());

		wvalid.write(false);
		wready.write(false);
		wlast.write(false);

		bvalid.write(true);
		bready.write(true);

		wait(clk.posedge_event());

		bvalid.write(false);
		bready.write(false);

		wack.write(true);

		wait(clk.posedge_event());

		wack.write(false);

		wait(clk.posedge_event());
	}

	void test_aw_w_ch_no_wvalid()
	{
		awvalid.write(true);
		awready.write(true);

		wait(clk.posedge_event());

		awvalid.write(false);
		awready.write(false);

		for (int i = 0; i < 200; i++) {
			wait(clk.posedge_event());
		}

		// Clean up
		wvalid.write(true);
		wready.write(true);
		wlast.write(true);

		wait(clk.posedge_event());

		wvalid.write(false);
		wready.write(false);
		wlast.write(false);

		bvalid.write(true);
		bready.write(true);

		wait(clk.posedge_event());

		bvalid.write(false);
		bready.write(false);

		wack.write(true);

		wait(clk.posedge_event());

		wack.write(false);

		wait(clk.posedge_event());
	}

	void test_aw_w_b_ch_no_bvalid()
	{
		awvalid.write(true);
		awready.write(true);

		wait(clk.posedge_event());

		awvalid.write(false);
		awready.write(false);

		wvalid.write(true);
		wready.write(true);
		wlast.write(true);

		wait(clk.posedge_event());

		wvalid.write(false);
		wready.write(false);
		wlast.write(false);

		for (int i = 0; i < 200; i++) {
			wait(clk.posedge_event());
		}

		// Clean up
		bvalid.write(true);
		bready.write(true);

		wait(clk.posedge_event());

		bvalid.write(false);
		bready.write(false);

		wack.write(true);

		wait(clk.posedge_event());

		wack.write(false);

		wait(clk.posedge_event());
	}

	void test_arvalid_toggle_before_arready()
	{
		arvalid.write(true);

		wait(clk.posedge_event());

		arvalid.write(false);

		// Clean up
		wait(clk.posedge_event());

		arready.write(true);

		wait(clk.posedge_event());

		arvalid.write(false);
		arready.write(false);

		rvalid.write(true);
		rready.write(true);

		wait(clk.posedge_event());

		rvalid.write(false);
		rready.write(false);

		wait(clk.posedge_event());

		rack.write(true);

		wait(clk.posedge_event());

		wack.write(false);

		wait(clk.posedge_event());
	}

	void test_ar_rr_no_rack()
	{
		arvalid.write(true);
		arready.write(true);

		wait(clk.posedge_event());

		arvalid.write(false);
		arready.write(false);

		rvalid.write(true);
		rready.write(true);

		wait(clk.posedge_event());

		rvalid.write(false);
		rready.write(false);

		for (int i = 0; i < 200; i++) {
			wait(clk.posedge_event());
		}

		rack.write(true);

		wait(clk.posedge_event());

		rack.write(false);

		wait(clk.posedge_event());
	}

	void test_aw_w_b_no_wack()
	{
		awvalid.write(true);
		awready.write(true);

		wait(clk.posedge_event());

		awvalid.write(false);
		awready.write(false);

		wvalid.write(true);
		wready.write(true);
		wlast.write(true);

		wait(clk.posedge_event());

		wvalid.write(false);
		wready.write(false);
		wlast.write(false);

		bvalid.write(true);
		bready.write(true);

		wait(clk.posedge_event());

		bvalid.write(false);
		bready.write(false);

		for (int i = 0; i < 200; i++) {
			wait(clk.posedge_event());
		}

		wack.write(true);

		wait(clk.posedge_event());

		wack.write(false);

		wait(clk.posedge_event());
	}

	void test_ac_no_cr()
	{
		acvalid.write(true);
		acready.write(true);

		wait(clk.posedge_event());

		acvalid.write(false);
		acready.write(false);

		for (int i = 0; i < 200; i++) {
			wait(clk.posedge_event());
		}

		crvalid.write(true);
		crready.write(true);

		wait(clk.posedge_event());

		crvalid.write(false);
		crready.write(false);

		wait(clk.posedge_event());
	}

	void test_ac_cr_no_cd()
	{
		acvalid.write(true);
		acready.write(true);

		wait(clk.posedge_event());

		acvalid.write(false);
		acready.write(false);

		crvalid.write(true);
		crready.write(true);
		crresp.write(1);

		wait(clk.posedge_event());

		crvalid.write(false);
		crready.write(false);
		crresp.write(0);

		for (int i = 0; i < 200; i++) {
			wait(clk.posedge_event());
		}

		cdvalid.write(true);
		cdready.write(true);

		for (int i = 0; i < 8; i++) {
			wait(clk.posedge_event());
		}

		cdvalid.write(false);
		cdready.write(false);

		wait(clk.posedge_event());
	}

};

SIGGEN_RUN(TestSuite)

ACEPCConfig checker_config()
{
	ACEPCConfig cfg;

	cfg.check_ace_handshakes();

	return cfg;
}

int sc_main(int argc, char *argv[])
{
	ACEProtocolChecker<AXI_ADDR_WIDTH, AXI_DATA_WIDTH>
			checker("checker", checker_config());

	ACESignals__ signals("ace-signals");

	SignalGen<AXI_ADDR_WIDTH, AXI_DATA_WIDTH> siggen("sig-gen");

	sc_clock clk("clk", sc_time(20, SC_US));
	sc_signal<bool> resetn("resetn", true);

	// Connect clk
	checker.clk(clk);
	siggen.clk(clk);

	// Connect reset
	checker.resetn(resetn);
	siggen.resetn(resetn);

	// Connect signals
	signals.connect(checker);
	signals.connect(siggen);

	sc_trace_file *trace_fp = sc_create_vcd_trace_file(argv[0]);

	sc_trace(trace_fp, siggen.clk, siggen.clk.name());
	signals.Trace(trace_fp);

	// Run
	sc_start(100, SC_MS);

	sc_stop();

	if (trace_fp) {
		sc_close_vcd_trace_file(trace_fp);
	}

	return 0;
}
