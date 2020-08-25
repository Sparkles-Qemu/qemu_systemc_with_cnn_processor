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
#include "checkers/pc-acelite.h"
#include "checkers/config-ace.h"
#include "test-modules/memory.h"
#include "test-modules/signals-acelite.h"
#include "siggen-acelite.h"

#define AXI_ADDR_WIDTH 64
#define AXI_DATA_WIDTH 64

typedef ACELiteSignals<
	AXI_ADDR_WIDTH,	// ADDR_WIDTH
	AXI_DATA_WIDTH	// DATA_WIDTH
> ACELiteSignals__;

SIGGEN_TESTSUITE(TestSuite)
{
	SIGGEN_TESTSUITE_CTOR(TestSuite)
	{}

	void run_tests()
	{
		SetMessageType(CHECKER_AXI_ERROR);

		TESTCASE(test_ar_rr_channel);
		TESTCASE(test_aw_w_b_channel);

		TESTCASE_NEG(test_toggle_araddr);
		TESTCASE_NEG(test_toggle_arprot);
		TESTCASE_NEG(test_toggle_aruser);
		TESTCASE_NEG(test_toggle_arregion);
		TESTCASE_NEG(test_toggle_arqos);
		TESTCASE_NEG(test_toggle_arcache);
		TESTCASE_NEG(test_toggle_arburst);
		TESTCASE_NEG(test_toggle_arsize);
		TESTCASE_NEG(test_toggle_arlen);
		TESTCASE_NEG(test_toggle_arid);
		TESTCASE_NEG(test_toggle_arlock);
		TESTCASE_NEG(test_toggle_araddr_after_3_cycles);
		TESTCASE_NEG(test_toggle_arprot_after_3_cycles);

		TESTCASE_NEG(test_toggle_rdata);
		TESTCASE_NEG(test_toggle_rresp);
		TESTCASE_NEG(test_toggle_ruser);
		TESTCASE_NEG(test_toggle_rid);
		TESTCASE_NEG(test_toggle_rdata_after_3_cycles);
		TESTCASE_NEG(test_toggle_rresp_after_3_cycles);

		TESTCASE_NEG(test_toggle_awaddr);
		TESTCASE_NEG(test_toggle_awprot);
		TESTCASE_NEG(test_toggle_awuser);
		TESTCASE_NEG(test_toggle_awregion);
		TESTCASE_NEG(test_toggle_awqos);
		TESTCASE_NEG(test_toggle_awcache);
		TESTCASE_NEG(test_toggle_awburst);
		TESTCASE_NEG(test_toggle_awsize);
		TESTCASE_NEG(test_toggle_awlen);
		TESTCASE_NEG(test_toggle_awid);
		TESTCASE_NEG(test_toggle_awlock);
		TESTCASE_NEG(test_toggle_awaddr_after_3_cycles);
		TESTCASE_NEG(test_toggle_awprot_after_3_cycles);

		TESTCASE_NEG(test_toggle_wdata);
		TESTCASE_NEG(test_toggle_wstrb);
		TESTCASE_NEG(test_toggle_wuser);
		TESTCASE_NEG(test_toggle_wlast);
		TESTCASE_NEG(test_toggle_wdata_after_3_cycles);
		TESTCASE_NEG(test_toggle_wstrb_after_3_cycles);

		TESTCASE_NEG(test_toggle_bresp);
		TESTCASE_NEG(test_toggle_buser);
		TESTCASE_NEG(test_toggle_bid);
		TESTCASE_NEG(test_toggle_bresp_after_3_cycles);
	}

	void test_ar_rr_channel()
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

		wait(clk.posedge_event());
	}

	void test_aw_w_b_channel()
	{
		awvalid.write(true);
		awready.write(true);

		wait(clk.posedge_event());

		awvalid.write(false);
		awready.write(false);

		wvalid.write(true);
		wready.write(true);

		wait(clk.posedge_event());

		wvalid.write(false);
		wready.write(false);

		bvalid.write(true);
		bready.write(true);

		wait(clk.posedge_event());

		bvalid.write(false);
		bready.write(false);

		wait(clk.posedge_event());
	}

#define GEN_TEST_TOGGLE_CH_SIG(ch, sig)	\
void test_toggle_ ## ch ## sig()	\
{					\
	ch ## sig.write(0x0);		\
	ch ## valid.write(true);	\
					\
	wait(clk.posedge_event());	\
					\
	ch ## sig.write(0x1);		\
	ch ## ready.write(true);	\
					\
	wait(clk.posedge_event());	\
					\
	ch ## valid.write(false);	\
	ch ## ready.write(false);	\
					\
	wait(clk.posedge_event());	\
}

#define GEN_TEST_TOGGLE_CH_SIG_AFTER_3_CYCLES(ch, sig)	\
void test_toggle_ ## ch ## sig ## _after_3_cycles()	\
{					\
	ch ## sig.write(0x0);		\
	ch ## valid.write(true);	\
					\
	wait(clk.posedge_event());	\
	wait(clk.posedge_event());	\
	wait(clk.posedge_event());	\
					\
	ch ## sig.write(0x1);		\
	ch ## ready.write(true);	\
					\
	wait(clk.posedge_event());	\
					\
	ch ## valid.write(false);	\
	ch ## ready.write(false);	\
					\
	wait(clk.posedge_event());	\
}

	//
	// Generate:
	// 	test_toggle_araddr
	// 	test_toggle_arprot
	// 	test_toggle_arprot_after_3_cycles
	//
	GEN_TEST_TOGGLE_CH_SIG(ar, addr)
	GEN_TEST_TOGGLE_CH_SIG(ar, prot)
	GEN_TEST_TOGGLE_CH_SIG(ar, user)
	GEN_TEST_TOGGLE_CH_SIG(ar, region)
	GEN_TEST_TOGGLE_CH_SIG(ar, qos)
	GEN_TEST_TOGGLE_CH_SIG(ar, cache)
	GEN_TEST_TOGGLE_CH_SIG(ar, burst)
	GEN_TEST_TOGGLE_CH_SIG(ar, size)
	GEN_TEST_TOGGLE_CH_SIG(ar, len)
	GEN_TEST_TOGGLE_CH_SIG(ar, id)
	GEN_TEST_TOGGLE_CH_SIG(ar, lock)
	GEN_TEST_TOGGLE_CH_SIG_AFTER_3_CYCLES(ar, addr)
	GEN_TEST_TOGGLE_CH_SIG_AFTER_3_CYCLES(ar, prot)

	//
	// Generate:
	// 	test_toggle_rdata
	// 	test_toggle_rresp
	//	...
	// 	test_toggle_rdata_after_3_cycles
	// 	test_toggle_rresp_after_3_cycles
	//
	GEN_TEST_TOGGLE_CH_SIG(r, data)
	GEN_TEST_TOGGLE_CH_SIG(r, resp)
	GEN_TEST_TOGGLE_CH_SIG(r, user)
	GEN_TEST_TOGGLE_CH_SIG(r, id)
	GEN_TEST_TOGGLE_CH_SIG_AFTER_3_CYCLES(r, data)
	GEN_TEST_TOGGLE_CH_SIG_AFTER_3_CYCLES(r, resp)

	//
	// Generate:
	// 	test_toggle_awaddr
	// 	test_toggle_awprot
	//	...
	// 	test_toggle_awaddr_after_3_cycles
	// 	test_toggle_awprot_after_3_cycles
	//
	GEN_TEST_TOGGLE_CH_SIG(aw, addr)
	GEN_TEST_TOGGLE_CH_SIG(aw, prot)
	GEN_TEST_TOGGLE_CH_SIG(aw, user)
	GEN_TEST_TOGGLE_CH_SIG(aw, region)
	GEN_TEST_TOGGLE_CH_SIG(aw, qos)
	GEN_TEST_TOGGLE_CH_SIG(aw, cache)
	GEN_TEST_TOGGLE_CH_SIG(aw, burst)
	GEN_TEST_TOGGLE_CH_SIG(aw, size)
	GEN_TEST_TOGGLE_CH_SIG(aw, len)
	GEN_TEST_TOGGLE_CH_SIG(aw, id)
	GEN_TEST_TOGGLE_CH_SIG(aw, lock)
	GEN_TEST_TOGGLE_CH_SIG_AFTER_3_CYCLES(aw, addr)
	GEN_TEST_TOGGLE_CH_SIG_AFTER_3_CYCLES(aw, prot)

	//
	// Generate:
	// 	test_toggle_wdata
	// 	test_toggle_wstrb
	//	...
	// 	test_toggle_wdata_after_3_cycles
	// 	test_toggle_wstrb_after_3_cycles
	//
	GEN_TEST_TOGGLE_CH_SIG(w, data)
	GEN_TEST_TOGGLE_CH_SIG(w, strb)
	GEN_TEST_TOGGLE_CH_SIG(w, user)
	GEN_TEST_TOGGLE_CH_SIG(w, last)
	GEN_TEST_TOGGLE_CH_SIG_AFTER_3_CYCLES(w, data)
	GEN_TEST_TOGGLE_CH_SIG_AFTER_3_CYCLES(w, strb)

	//
	// Generate:
	// 	test_toggle_bresp
	// 	test_toggle_bresp_after_3_cycles
	//
	GEN_TEST_TOGGLE_CH_SIG(b, resp)
	GEN_TEST_TOGGLE_CH_SIG(b, user)
	GEN_TEST_TOGGLE_CH_SIG(b, id)
	GEN_TEST_TOGGLE_CH_SIG_AFTER_3_CYCLES(b, resp)
};

SIGGEN_RUN(TestSuite)

ACEPCConfig checker_config()
{
	ACEPCConfig cfg;

	cfg.check_stable_data_resp_signal();

	return cfg;
}

int sc_main(int argc, char *argv[])
{
	ACELiteProtocolChecker<AXI_ADDR_WIDTH, AXI_DATA_WIDTH>
			checker("checker", checker_config());

	ACELiteSignals__ signals("acelite-signals");

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
