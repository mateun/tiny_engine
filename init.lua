-- ========== Bootstrap lazy.nvim ==========
local lazypath = vim.fn.stdpath("data") .. "/lazy/lazy.nvim"
if not vim.loop.fs_stat(lazypath) then
  vim.fn.system({ "git", "clone", "--filter=blob:none",
    "https://github.com/folke/lazy.nvim.git", lazypath })
end
vim.opt.rtp:prepend(lazypath)

require("lazy").setup({
  { "neovim/nvim-lspconfig" },
  { "hrsh7th/nvim-cmp",
    dependencies = {
      "hrsh7th/cmp-nvim-lsp",
      "L3MON4D3/LuaSnip",
      "saadparwaiz1/cmp_luasnip",
    },
  },
})

-- ========== nvim-cmp ==========
local cmp = require("cmp")
cmp.setup({
  snippet = { expand = function(args) require("luasnip").lsp_expand(args.body) end },
  mapping = cmp.mapping.preset.insert({
    ["<C-Space>"] = cmp.mapping.complete(),
    ["<CR>"]      = cmp.mapping.confirm({ select = true }),
    ["<Tab>"]     = cmp.mapping.select_next_item(),
    ["<S-Tab>"]   = cmp.mapping.select_prev_item(),
  }),
  sources = { { name = "nvim_lsp" }, { name = "buffer" } },
})
vim.o.completeopt = "menu,menuone,noselect"

-- ========== clangd (single setup) ==========
local lspconfig = require("lspconfig")
local caps = require("cmp_nvim_lsp").default_capabilities()

local function on_attach(_, bufnr)
  local map = function(mode, lhs, rhs, desc)
    vim.keymap.set(mode, lhs, rhs, { buffer = bufnr, silent = true, desc = desc })
  end
  map("n", "gd", vim.lsp.buf.definition,      "Go to definition")
  map("n", "gD", vim.lsp.buf.declaration,     "Go to declaration")
  map("n", "gr", vim.lsp.buf.references,      "Find references")
  map("n", "gi", vim.lsp.buf.implementation,  "Go to implementation")
  map("n", "K",  vim.lsp.buf.hover,           "Hover docs")
  map("n", "<leader>rn", vim.lsp.buf.rename,  "Rename symbol")
  map("n", "<leader>ca", vim.lsp.buf.code_action, "Code action")
end

lspconfig.clangd.setup({
  cmd = {
    "C:/Program Files/LLVM/bin/clangd.exe",
    "--background-index",
    "--clang-tidy",
    "--header-insertion=iwyu",
    "--query-driver=C:/Program Files/Microsoft Visual Studio/*/VC/Tools/MSVC/*/bin/Hostx64/x64/cl.exe",
  },
  capabilities = caps,
  on_attach = on_attach,
})

vim.o.shiftwidth = 4
vim.o.tabstop = 4
vim.o.softtabstop = 4
vim.o.expandtab = true  -- optional: insert spaces instead of tabs

vim.keymap.set("n", "<F5>", function()
  vim.cmd("!build.bat")
end, { desc = "Build with cl" })

vim.keymap.set("n", "<F6>", function()
  vim.cmd("!build\\tiny_engine_sample.exe")
end, { desc = "Run sample program" })

vim.cmd.colorscheme("slate")
