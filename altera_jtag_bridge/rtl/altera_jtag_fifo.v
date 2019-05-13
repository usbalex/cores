
// synthesis translate_off
`timescale 1ns / 1ps
// synthesis translate_on


module altera_jtag_fifo (
  // inputs
  clk,
  rst_n,
  write,
  writedata,
  // outputs
  readdata,
  dataavailable,
  readyfordata

  ,
  debug
)
  /* synthesis ALTERA_ATTRIBUTE = "SUPPRESS_DA_RULE_INTERNAL=\"R101,C106,D101,D103\"" */ ;

  input         clk;
  input         rst_n;
  input         write;
  input   [7:0] writedata;
  output  [7:0] readdata;
  output        dataavailable;
  output        readyfordata;

  output  [7:0] debug;

  wire    [7:0] readdata;
  wire          dataavailable;
  wire          readyfordata;

  wire          fifo_clear;

  wire    [7:0] rfifo_wdata;
  wire          rfifo_empty;
  wire          rfifo_full;
  wire    [7:0] rfifo_rdata;
  wire          rfifo_rd;
  wire          rfifo_wr;
//  wire    [5:0] rfifo_used;

  reg     [7:0] wfifo_wdata;
  wire          wfifo_empty;
  wire          wfifo_full;
  wire    [7:0] wfifo_rdata;
  wire          wfifo_rd;
  reg           wfifo_wr;
//  wire    [5:0] wfifo_used;

  wire          r_ena;
  wire          t_ena;
  reg           r_val;
  reg           t_dav;
  wire          t_pause;


  assign fifo_clear = ~rst_n;
  assign wfifo_rd = r_ena & ~wfifo_empty;
  assign rfifo_wr = t_ena & ~rfifo_full;

  assign dataavailable  = ~rfifo_empty;
  assign readyfordata   = ~wfifo_full;
  assign rfifo_rd       = 1'b1;
  assign readdata       = rfifo_rdata;


  // from jtag chain
  always @(posedge clk or negedge rst_n)
    begin
      if (rst_n == 0)
        begin
          r_val <= 1'b0;
          t_dav <= 1'b1;
        end
      else 
        begin
          r_val <= r_ena & ~wfifo_empty;
          t_dav <= ~rfifo_full;
        end
    end

  // to jtag chain
  always @(posedge clk or negedge rst_n)
    begin
      if (rst_n == 0)
        begin
          wfifo_wr <= 1'b0;
//          wfifo_wdata <= 8'b00000001;
        end
      else 
        begin
          wfifo_wr <= 1'b0;
          // write
          if (write) begin
            wfifo_wr    <= ~wfifo_full;
            wfifo_wdata <= writedata;
          end
//            wfifo_wr    <= wfifo_empty;
//            wfifo_wdata <= wfifo_wdata + 8'b00000001;
        end
    end


  // to jtag chain
  scfifo the_wfifo (
    .aclr  (fifo_clear),
    .clock (clk),
    .data  (wfifo_wdata),
    .empty (wfifo_empty),
    .full  (wfifo_full),
    .q     (wfifo_rdata),
    .rdreq (wfifo_rd),
//    .usedw (wfifo_used),
    .wrreq (wfifo_wr)
  );
  defparam the_wfifo.lpm_hint = "RAM_BLOCK_TYPE=AUTO",
           the_wfifo.lpm_numwords = 64,
           the_wfifo.lpm_showahead = "OFF",
           the_wfifo.lpm_type = "scfifo",
           the_wfifo.lpm_width = 8,
           the_wfifo.lpm_widthu = 6,
           the_wfifo.overflow_checking = "OFF",
           the_wfifo.underflow_checking = "OFF",
           the_wfifo.use_eab = "ON";
  

  // from jtag chain
  scfifo the_rfifo (
    .aclr  (fifo_clear),
    .clock (clk),
    .data  (rfifo_wdata),
    .empty (rfifo_empty),
    .full  (rfifo_full),
    .q     (rfifo_rdata),
    .rdreq (rfifo_rd),
//    .usedw (rfifo_used),
    .wrreq (rfifo_wr)
  );
  defparam the_rfifo.lpm_hint = "RAM_BLOCK_TYPE=AUTO",
           the_rfifo.lpm_numwords = 64,
           the_rfifo.lpm_showahead = "OFF",
           the_rfifo.lpm_type = "scfifo",
           the_rfifo.lpm_width = 8,
           the_rfifo.lpm_widthu = 6,
           the_rfifo.overflow_checking = "OFF",
           the_rfifo.underflow_checking = "OFF",
           the_rfifo.use_eab = "ON";
  

  alt_jtag_atlantic the_alt_jtag_atlantic (
    .clk (clk),
    .r_dat (wfifo_rdata), // (in)  input data to send out via jtag
    .r_ena (r_ena),       // (out) ready to accept input data
    .r_val (r_val),       // (in)  input data valid (should only be set high, if r_ena is high)
    .rst_n (rst_n),
    .t_dat (rfifo_wdata), // (out) output data received via jtag
    .t_dav (t_dav),       // (in)  read fifo ready to accept data? (would be 'r_ena' of rfifo)
    .t_ena (t_ena),        // (out) output data valid
    .t_pause (t_pause)    // (out) (currently transmitting via jtag??? -> don't read or write data???)
  );
  defparam the_alt_jtag_atlantic.INSTANCE_ID = 0,
           the_alt_jtag_atlantic.LOG2_RXFIFO_DEPTH = 6,
           the_alt_jtag_atlantic.LOG2_TXFIFO_DEPTH = 6,
           the_alt_jtag_atlantic.SLD_AUTO_INSTANCE_INDEX = "YES";


  reg w_reg;
  reg not_full;
  reg not_empty;
  always @(posedge clk or negedge rst_n)
    begin
      if (rst_n == 0)
        begin
          w_reg <= 1'b0;
          not_full <= 1'b0;
          not_empty <= 1'b0;
        end
      else 
        begin
          if (t_ena) begin
            w_reg <= 1'b1;
          end
          if (~rfifo_full) begin
            not_full <= 1'b1;
          end
          if (~rfifo_empty) begin
            not_empty <= 1'b1;
          end
        end
    end
  reg rf_ne;
  reg rf_f;
  reg rf_wr;
  reg rf_rd;
  always @(posedge clk or negedge rst_n)
    begin
      if (rst_n == 0)
        begin
          rf_ne <= 1'b0;
          rf_f  <= 1'b0;
          rf_wr <= 1'b0;
          rf_rd <= 1'b0;
        end
      else 
        begin
          if (~rfifo_empty) begin
            rf_ne <= 1'b1;
          end
          if (rfifo_full) begin
            rf_f  <= 1'b1;
          end
          if (rfifo_wr) begin
            rf_wr <= 1'b1;
          end
          if (rfifo_rd) begin
            rf_rd <= 1'b1;
          end
        end
    end

  //assign debug = {w_reg, not_full, not_empty, t_pause, rfifo_wdata[0], rfifo_empty, rfifo_full, rfifo_rd};


  reg wf_ne;
  reg wf_f;
  reg wf_wr;
  reg wf_rd;
  always @(posedge clk or negedge rst_n)
    begin
      if (rst_n == 0)
        begin
          wf_ne <= 1'b0;
          wf_f  <= 1'b0;
          wf_wr <= 1'b0;
          wf_rd <= 1'b0;
        end
      else 
        begin
          if (~wfifo_empty) begin
            wf_ne <= 1'b1;
          end
          if (wfifo_full) begin
            wf_f  <= 1'b1;
          end
          if (wfifo_wr) begin
            wf_wr <= 1'b1;
          end
          if (wfifo_rd) begin
            wf_rd <= 1'b1;
          end
        end
    end
  //assign debug = {wfifo_empty, wfifo_full, wfifo_rd, wfifo_wr, wfifo_rdata[1:0], wfifo_wdata[1:0]};
  assign debug = {wf_ne, wf_f, wf_wr, wf_rd, rf_ne, rf_f, rf_wr, rf_rd};

endmodule

