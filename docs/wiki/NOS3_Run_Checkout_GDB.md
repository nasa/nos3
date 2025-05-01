
# Scenario - Sample Debug with GDB

This scenario demonstrates how to launch the **sample simulator** and its **checkout application** with **GDB** for debugging inside the NOS3 environment.  
It builds on the standard sample simulation scenario and integrates `gdb` into the launch process to allow real-time inspection, breakpointing, and step-through debugging of the `sample_checkout` application.

This is useful for:
* Debugging the sample payload logic
* Inspecting memory or variable values during runtime
* Tracing faults or validating application behavior

---

## Learning Goals

By the end of this scenario, you should be able to:

* Launch the sample checkout application within a `gdb` session inside Docker
* Set breakpoints and run the application interactively
* Inspect variables and function call flow at runtime
* Integrate low-level debugging into a NOS3 simulation workflow

---

## Prerequisites

Before running this scenario, ensure the following:

* You have completed NOS3 installation from [Getting Started](./Getting_Started.md)
* The `sample_checkout` binary has been built
  * You can verify this by checking that `components/sample/fsw/standalone/build/sample_checkout` exists
* You are working from the top level of the NOS3 repository

---

## Launch Instructions

### Step 1: Build NOS3

```bash
make
```

### Step 2: (Optional) Launch the Full Scenario

If you want to bring up the entire simulation environment:

```bash
make launch
```

You may minimize the NOS3 Launcher, but do not close it.

---

## Step 3: Launch the Sample Checkout App in GDB

Open a new terminal and run the following `gnome-terminal` command to launch the `sample_checkout` binary inside `gdb`:

```bash
gnome-terminal --title="Sample Checkout (gdb)" -- $DFLAGS -it \
  -v $BASE_DIR:$BASE_DIR \
  --name $SC_NUM"_sample_checkout_debug" \
  --network=$SC_NETNAME \
  -w $BASE_DIR $DBOX \
  gdb ./components/sample/fsw/standalone/build/sample_checkout
```

> 🔧 **Note:** This assumes your environment variables (e.g., `$DFLAGS`, `$BASE_DIR`, `$SC_NUM`, `$DBOX`, `$SC_NETNAME`) are set as in a typical NOS3 session.

---

## Step 4: Use GDB

Once inside the GDB session, you can:

* Set breakpoints:
  ```gdb
  break main
  ```
* Run the application:
  ```gdb
  run
  ```
* Step through code:
  ```gdb
  next
  step
  ```
* Inspect variables:
  ```gdb
  print variable_name
  info locals
  ```

To quit GDB:
```gdb
quit
```

---

## Optional: Enable TUI Mode

If your container supports `ncurses`, you can add `-tui` to enable the GDB text-based UI:

```bash
gdb -tui ./components/sample/fsw/standalone/build/sample_checkout
```

---

## Conclusion

You have now successfully launched a NOS3 sample component with `gdb` attached, allowing real-time interactive debugging. This is a powerful way to test payload logic and simulate edge cases directly inside your NOS3 development loop.
